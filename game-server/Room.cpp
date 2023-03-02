#include "Room.h"
#include "net/protocol/game_packet_generated.h"
#include "util/GroupRoutine.h"
#include "photon/thread/thread11.h"
#include "photon/common/alog.h"
#include <algorithm>
#include <random>

Room::Room(int roomId, const std::vector<UserInfo>& users, game::RoomSetting setting) 
    : mId(roomId), mStatusCode(game::GameStatusCode_Wait), mWorldSimulator(new Simulator(0, this, setting.world_gravity())), mSetting(setting), mCreationTime(std::chrono::system_clock::now()) { 
    
    auto& spawnPoints = mWorldSimulator->getMap()->getSpawnPoints();

    int index = 0;
    for (const auto& user : users) {
        auto& spawnPoint = spawnPoints[index++];
        if (index >= spawnPoints.size()) {
            index = 0;
        }

        auto player = mWorldSimulator->spawnPlayer(spawnPoint, mSetting.initial_player_hp(), mSetting.player_ground_movement_force());

        mUserList.insert_or_assign(player->getEntityId(), new RoomUser{false, player, user.connection, user.nickname, std::chrono::system_clock::now(), std::chrono::system_clock::now(), 0, {}});
        mConnectionMap[user.connection.get()] = player->getEntityId();
    }
}

inline game::Vec2 convertVec2(const simVec2& vec2) {
    return game::Vec2(vec2.x, vec2.y);
}

inline game::EntityStatus toEntityStatus(IEntity* entity) {
    return game::EntityStatus(
        entity->getEntityId(),
        entity->getHp(),
        convertVec2(entity->getPosition()),
        convertVec2(entity->getLinearVelocity())
    );
}

void Room::disconnectClient(int userId, Connection* connection) {
    auto userIter = mUserList.find(userId);
    
    if (userIter == mUserList.end()) {
        return;
    }
    LOG_INFO("disconnect room user ", userId);
    auto winnerIter = std::find_if_not(mUserList.begin(), mUserList.end(), [=](const std::pair<int, RoomUser*>& d) {
        return d.first == userId;
    });

    mRun = false;
    mWinnerId = winnerIter->first;
}

int Room::getRoomId() {
    return mId;
}

void Room::onPlayerHit(Player* player) {

    if (player->getHp() <= 0) {
        auto winnerEntry = std::find_if(mUserList.begin(), mUserList.end(), [=](const std::pair<int, RoomUser*>& entry) {
            return entry.second->userEntity->getEntityId() != player->getEntityId();
        });

        assert(winnerEntry != mUserList.end());

        mRun = false;
        mWinnerId = winnerEntry->second->userEntity->getEntityId();
        return;
    }
}

void Room::onPlayerContactMap(Player* player, IMap* map) {

}

void Room::onPlayerHitGunshot(Player* player, GunShot* gunshot) {
    for(auto& entry : mUserList) {
        entry.second->packetBuilder.addCollisionEventPushMessage(player->getEntityType() | gunshot->getEntityType(), toEntityStatus(player), toEntityStatus(gunshot));
    }
    onPlayerHit(player);
}

void Room::onRockfallHitGunshot(Rockfall* rockfall, GunShot* gunshot) {
    for(auto& entry : mUserList) {
        entry.second->packetBuilder.addCollisionEventPushMessage(rockfall->getEntityType() | gunshot->getEntityType(), toEntityStatus(rockfall), toEntityStatus(gunshot));
    }
}

void Room::onHitRockfall(Player* player, Rockfall* rockfall) {
    for(auto& entry : mUserList) {
        entry.second->packetBuilder.addCollisionEventPushMessage(player->getEntityType() | rockfall->getEntityType(), toEntityStatus(player), toEntityStatus(rockfall));
    }
    onPlayerHit(player);
}

void Room::onPickupHp(Player* player, HpItem* item) {
    for(auto& entry : mUserList) {
        entry.second->packetBuilder.addCollisionEventPushMessage(player->getEntityType() | item->getEntityType(), toEntityStatus(player), toEntityStatus(item));
    }
}

void Room::syncUser(RoomUser* user) {
    auto& entityMap = mWorldSimulator->getEntityMap();

    std::vector<game::UserStatus> userStatus(mUserList.size());

    std::transform(mUserList.begin(), mUserList.end(), userStatus.begin(), [=](const decltype(mUserList)::iterator::value_type& entry) {
        return game::UserStatus{
            entry.first,
            user->userEntity->getEntityId() == entry.first,
            true,
            entry.second->currentPing,
        };
    });

    user->packetBuilder.addGameStatusPushMessage(mStatusCode, userStatus);
    for (auto& entry : entityMap) {
        user->packetBuilder.addSpawnEntityPushMessage((game::EntityType)entry.second->getEntityType(), toEntityStatus(entry.second));
    }
}

void Room::processClientPacket(int userId, const game::Packet* packet) {

    auto userIter = mUserList.find(userId);
    if (userIter == mUserList.end()) {
        return;
    }         

    RoomUser* user = userIter->second;

    const auto& messages = *(packet->messages());
    auto messageTypes = packet->messages_type();
    int index = 0;
    for (auto message : messages) {

        switch(messageTypes->Get(index++)) {
        case game::Payload_PingAck: {
            auto ping = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - user->lastPingTime);
            user->currentPing = ping.count()/2;
            break;
        }
        case game::Payload_BattleReadyReq:
            user->battleReady = true;    
            break;
        case game::Payload_ChangePlayerStatusReq: {
            
            if (mStatusCode != game::GameStatusCode_Start) {
                return;
            }

            auto playerStatus = reinterpret_cast<const game::ChangePlayerStatusReq*>(message);

            auto player = reinterpret_cast<Player*>(user->userEntity);

            auto entity = player;

            int actionCount = 0;
            auto allowedActions = std::array<uint8_t, 5>();

            for (auto action : *playerStatus->action()) {
                switch (action) {
                case game::EntityAction_Left:
                    player->setMovementStatus(PlayerMovementStatus_Left);
                    allowedActions[actionCount++] = action;
                    break;
                case game::EntityAction_Right:
                    player->setMovementStatus(PlayerMovementStatus_Right);
                    allowedActions[actionCount++] = action;
                    break;
                case game::EntityAction_Stop:
                    player->setMovementStatus(PlayerMovementStatus_Stop);
                    allowedActions[actionCount++] = action;
                    break;
                case game::EntityAction_Jump:
                    if (player->jump(simVec2(player->getLinearVelocity().x, mSetting.player_jump_velocity()))) {
                        allowedActions[actionCount++] = action;
                    }
                    break;
                case game::EntityAction_Shot:
                    auto now = std::chrono::system_clock::now();

                    if (std::chrono::duration_cast<std::chrono::seconds>(now - user->lastShotTime).count() >= mSetting.player_shot_cooltime()) {
                        mWorldSimulator->spawnPlayerGunShot(player, mSetting.gunshot_damage());
                        allowedActions[actionCount++] = action;
                        user->lastShotTime = now;
                    }
                    
                    break;
                }
            }

            for (auto& user : mUserList) {
                user.second->packetBuilder.addChangeEntityStatusPushMessage(allowedActions.data(), actionCount, toEntityStatus(player));
            }
        }
        }
    }
}

int Room::startRoomSimulation() {

    GroupRoutine groupRoutine;

    // sender routine
    for (auto& user : mUserList) {
        auto roomUser = user.second;

        groupRoutine.loopWithGroup([&](){
            user.second->packetBuilder.waitPacket();
            photon::thread_usleep(15 * 1000);
            user.second->packetBuilder.sendPacketAndReset(user.second->socketConnection.get());
        });
    }

    // ping routine
    groupRoutine.loopWithGroup([&]() {
        for (auto& user : mUserList) {
            auto& packetBuilder = user.second->packetBuilder;
            user.second->lastPingTime = std::chrono::system_clock::now();
            packetBuilder.addPingPushMessage(user.second->currentPing);
        }
        photon::thread_sleep(3);
    });

    bool clientReady = false;
    do {
        photon::thread_usleep(500 * 1000);
        clientReady = std::all_of(mUserList.begin(), mUserList.end(), [](const std::pair<int, RoomUser*>& pair){
            return pair.second->battleReady;
        });
    } while(!clientReady && mRun);

    LOG_INFO("room ", mId, " all client is ready");

    mStatusCode = game::GameStatusCode_Countdown;
    for (auto& user : mUserList) {
        syncUser(user.second);
    }

    photon::thread_sleep(mSetting.starting_countdown_second());

    
    std::random_device rd;

    // spawn routine
    groupRoutine.loopWithGroup([&]() {
        photon::thread_sleep(5);
        int count = rd() % 3 + 1;

        for (int iter = 0; iter < count; iter++) {
            auto rockfall = mWorldSimulator->spawnRockfall(simVec2(rd() % 18 + 2, 9), this->mSetting.rockfalll_damage());
            for (auto& user : mUserList) {
                auto& packetBuilder = user.second->packetBuilder;
                packetBuilder.addSpawnEntityPushMessage(game::EntityType_Rockfall, toEntityStatus(rockfall));
            }
        }
    });

    mStatusCode = game::GameStatusCode_Start;
    for (auto& user : mUserList) {
        auto& packetBuilder = user.second->packetBuilder;
        packetBuilder.addGameStatusPushMessage(game::GameStatusCode_Start, {});
        packetBuilder.sendPacketAndReset(user.second->socketConnection.get());
    }

    float frame = mSetting.frame_rate();
    float fixedDelta = 1.0f/frame;
    float delta = fixedDelta;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = std::chrono::high_resolution_clock::now();

    // main simulation loop
    while(mRun) {

        startTime = std::chrono::high_resolution_clock::now();

        photon::thread_usleep((int)(fixedDelta * 1000000));
        endTime = std::chrono::high_resolution_clock::now();

        delta = (float)(double(std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime).count()) / 1000.0f);

        mWorldSimulator->simulate(delta);
    }

    mStatusCode = game::GameStatusCode_End;

    LOG_INFO("room ", mId, " game is end");

    for (auto& entry : mUserList) {
        entry.second->packetBuilder.releaseAll();    
        entry.second->packetBuilder.addGameResultPushMessage(game::GameStatusCode::GameStatusCode_End, mWinnerId);
        entry.second->packetBuilder.sendPacketAndReset(entry.second->socketConnection.get());
    }

    groupRoutine.stopAndWait();
    
    return mWinnerId;
}

int Room::getUserId(Connection* connection) {
    return mConnectionMap[connection];
}

const game::RoomSetting& Room::getRoomSetting() {
    return mSetting;
}

const std::map<int, RoomUser*>& Room::getUsers() {
    return mUserList;
}

Room::~Room() {
    for (auto pair : mUserList) {
        delete pair.second;
    }

    delete mWorldSimulator;
}