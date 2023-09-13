#include "Room.h"
#include "net/protocol/game_packet_generated.h"
#include "util/GroupRoutine.h"
#include "photon/thread/thread11.h"
#include "photon/common/alog.h"
#include <algorithm>
#include <random>

Room::Room(int roomId, const std::vector<User*>& users, game::RoomSetting setting) 
    : mId(roomId), mStatusCode(game::GameStatusCode_Wait), mWorldSimulator(new Simulator(0, this, setting.world_gravity())), mSetting(setting), mCreationTime(std::chrono::system_clock::now()) { 
    
    auto& spawnPoints = mWorldSimulator->getMap()->getSpawnPoints();

    int index = 0;
    for (User* user : users) {
        auto& spawnPoint = spawnPoints[index++];
        if (index >= spawnPoints.size()) {
            index = 0;
        }

        auto player = mWorldSimulator->spawnPlayer(spawnPoint, mSetting.initial_player_hp(), mSetting.player_ground_movement_force());

        mUserMap.insert_or_assign(player->getEntityId(), new RoomUser{false, user});
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

void Room::disconnectUser(User* user) {
    int userId = user->GetPlayer()->getEntityId();
    auto userIter = mUserMap.find(userId);
    
    if (userIter == mUserMap.end()) {
        return;
    }
    LOG_INFO("disconnect room user ", userId);
    auto winnerIter = std::find_if_not(mUserMap.begin(), mUserMap.end(), [=](const std::pair<int, RoomUser*>& d) {
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
        auto winnerEntry = std::find_if(mUserMap.begin(), mUserMap.end(), [=](const std::pair<int, RoomUser*>& entry) {
            return entry.second->user->GetPlayer()->getEntityId() != player->getEntityId();
        });

        assert(winnerEntry != mUserMap.end());

        mRun = false;
        mWinnerId = winnerEntry->second->user->GetPlayer()->getEntityId();
        return;
    }
}

void Room::onPlayerContactMap(Player* player, IMap* map) {

}

void Room::onPlayerHitGunshot(Player* player, GunShot* gunshot) {
    for(auto& entry : mUserMap) {
        entry.second->user->GetPacketBuilder()->addCollisionEventPushMessage(player->getEntityType() | gunshot->getEntityType(), toEntityStatus(player), toEntityStatus(gunshot));
    }
    onPlayerHit(player);
}

void Room::onRockfallHitGunshot(Rockfall* rockfall, GunShot* gunshot) {
    for(auto& entry : mUserMap) {
        entry.second->user->GetPacketBuilder()->addCollisionEventPushMessage(rockfall->getEntityType() | gunshot->getEntityType(), toEntityStatus(rockfall), toEntityStatus(gunshot));
    }
}

void Room::onHitRockfall(Player* player, Rockfall* rockfall) {
    for(auto& entry : mUserMap) {
        entry.second->user->GetPacketBuilder()->addCollisionEventPushMessage(player->getEntityType() | rockfall->getEntityType(), toEntityStatus(player), toEntityStatus(rockfall));
    }
    onPlayerHit(player);
}

void Room::onPickupHp(Player* player, HpItem* item) {
    for(auto& entry : mUserMap) {
        entry.second->user->GetPacketBuilder()->addCollisionEventPushMessage(player->getEntityType() | item->getEntityType(), toEntityStatus(player), toEntityStatus(item));
    }
}

void Room::syncUser(RoomUser* user) {
    auto& entityMap = mWorldSimulator->getEntityMap();

    std::vector<game::UserStatus> userStatus(mUserMap.size());

    std::transform(mUserMap.begin(), mUserMap.end(), userStatus.begin(), [=](const decltype(mUserMap)::iterator::value_type& entry) {
        return game::UserStatus{
            entry.first,
            user->user->GetPlayer()->getEntityId() == entry.first,
            true,
            entry.second->user->GetPlayer()->getLastPingTime(),
        };
    });

    user->user->GetPacketBuilder()->addGameStatusPushMessage(mStatusCode, userStatus);
    for (auto& entry : entityMap) {
        user->user->GetPacketBuilder()->addSpawnEntityPushMessage((game::EntityType)entry.second->getEntityType(), toEntityStatus(entry.second));
    }
}

int Room::startRoomSimulation() {

    GroupRoutine groupRoutine;

    // sender routine
    for (auto& user : mUserMap) {
        auto roomUser = user.second;

        groupRoutine.loopWithGroup([&](){
            user.second->user->GetPacketBuilder()->waitPacket();
            user.second->user->GetPacketBuilder()->sendPacketAndReset();
        });
    }

    // ping routine
    /*
    groupRoutine.loopWithGroup([&]() {
        for (auto& user : mUserMap) {
            auto packetBuilder = user.second->user->GetPacketBuilder();
            user.second->lastPingTime = std::chrono::system_clock::now();
            packetBuilder.addPingPushMessage(user.second->currentPing);
        }
        photon::thread_sleep(3);
    });
    */

    bool clientReady = false;
    do {
        photon::thread_usleep(500 * 1000);
        clientReady = std::all_of(mUserMap.begin(), mUserMap.end(), [](const std::pair<int, RoomUser*>& pair){
            return pair.second->battleReady;
        });
    } while(!clientReady && mRun);

    LOG_INFO("room ", mId, " all client is ready");

    mStatusCode = game::GameStatusCode_Countdown;
    for (auto& user : mUserMap) {
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
            for (auto& user : mUserMap) {
                auto packetBuilder = user.second->user->GetPacketBuilder();
                packetBuilder->addSpawnEntityPushMessage(game::EntityType_Rockfall, toEntityStatus(rockfall));
            }
        }
    });

    mStatusCode = game::GameStatusCode_Start;
    for (auto& user : mUserMap) {
        auto packetBuilder = user.second->user->GetPacketBuilder();
        packetBuilder->addGameStatusPushMessage(game::GameStatusCode_Start, {});
        packetBuilder->sendPacketAndReset();
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

    for (auto& entry : mUserMap) {
        entry.second->user->GetPacketBuilder()->releaseAll();    
        entry.second->user->GetPacketBuilder()->addGameResultPushMessage(game::GameStatusCode::GameStatusCode_End, mWinnerId);
        entry.second->user->GetPacketBuilder()->sendPacketAndReset();
    }

    groupRoutine.stopAndWait();
    
    return mWinnerId;
}

const game::RoomSetting& Room::getRoomSetting() {
    return mSetting;
}

const std::map<int, RoomUser*>& Room::getUsers() {
    return mUserMap;
}

Room::~Room() {
    for (auto pair : mUserMap) {
        delete pair.second;
    }

    delete mWorldSimulator;
}