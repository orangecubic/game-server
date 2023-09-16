#include "Room.h"
#include "net/protocol/game_packet_generated.h"
#include "util/GroupRoutine.h"
#include "photon/thread/thread11.h"
#include "photon/common/alog.h"
#include <algorithm>
#include <random>

game::Vec2 convertVec2(const simVec2& vec2) {
    return game::Vec2(vec2.x, vec2.y);
}

game::EntityStatus toEntityStatus(IEntity* entity) {
    return game::EntityStatus(
        entity->getEntityId(),
        entity->getHp(),
        convertVec2(entity->getPosition()),
        convertVec2(entity->getLinearVelocity())
    );
}

Room::Room(int roomId, const std::vector<User*>& users, game::RoomSetting setting) 
    : mId(roomId), mStatusCode(game::GameStatusCode_Wait), mWorldSimulator(new Simulator(this, 0, this, setting.world_gravity())), mSetting(setting), mCreationTime(std::chrono::system_clock::now()) { 
    
    auto& spawnPoints = mWorldSimulator->getMap()->getSpawnPoints();

    int index = 0;
    for (User* user : users) {
        auto& spawnPoint = spawnPoints[index++];
        if (index >= spawnPoints.size()) {
            index = 0;
        }

        auto player = mWorldSimulator->spawnPlayer(spawnPoint, mSetting.initial_player_hp(), mSetting.player_ground_movement_force());

        mUserMap.insert_or_assign(player->getEntityId(), new RoomUser{false, user});
        mPlayerMap.insert_or_assign(user, player);
    }
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

void Room::readyForBattle(User* user) {
    Player* player = user->GetPlayer();
    if (player == nullptr)
        return;
    mUserMap[player->getEntityId()]->battleReady = true;
}

int Room::getRoomId() {
    return mId;
}

void Room::onPlayerHit(Player* player) {

    LOG_INFO("player hit least hp is ", player->getHp());

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

void Room::onPlayerContactMap(Player* player, IMap* map) { }

void Room::onPlayerHitGunshot(Player* player, GunShot* gunshot) {

    broadcastPacket(
        SharedPacketBuilder.addCollisionEventPushMessage(player->getEntityType() | gunshot->getEntityType(), toEntityStatus(player), toEntityStatus(gunshot)).buildPacket()
    );
    onPlayerHit(player);
}

void Room::onRockfallHitGunshot(Rockfall* rockfall, GunShot* gunshot) {
    broadcastPacket(
        SharedPacketBuilder.addCollisionEventPushMessage(rockfall->getEntityType() | gunshot->getEntityType(), toEntityStatus(rockfall), toEntityStatus(gunshot)).buildPacket()
    );
}

void Room::onHitRockfall(Player* player, Rockfall* rockfall) {
    broadcastPacket(
        SharedPacketBuilder.addCollisionEventPushMessage(player->getEntityType() | rockfall->getEntityType(), toEntityStatus(player), toEntityStatus(rockfall)).buildPacket()
    );
    onPlayerHit(player);
}

void Room::onPickupHp(Player* player, HpItem* item) {
    broadcastPacket(
        SharedPacketBuilder.addCollisionEventPushMessage(player->getEntityType() | item->getEntityType(), toEntityStatus(player), toEntityStatus(item)).buildPacket()
    );
}

void Room::syncUser() {
    auto& entityMap = mWorldSimulator->getEntityMap();
    
    for (auto entry : mUserMap) {

        std::vector<game::UserStatus> userStatus(mUserMap.size());

        std::transform(mUserMap.begin(), mUserMap.end(), userStatus.begin(), [=](const decltype(mUserMap)::iterator::value_type& entry) {
            return game::UserStatus{
                entry.first,
                entry.second->user->GetPlayer()->getEntityId() == entry.first,
                true,
                entry.second->user->GetPlayer()->getPing(),
            };
        });

        SharedPacketBuilder.addGameStatusPushMessage(mStatusCode, userStatus).sendPacketAndReset(entry.second->user);
    }

    auto& builder = SharedPacketBuilder;
    for (auto& entry : entityMap) {
        builder.addSpawnEntityPushMessage((game::EntityType)entry.second->getEntityType(), toEntityStatus(entry.second));
    }
    broadcastPacket(builder.buildPacket());
}

int Room::startRoomSimulation() {

    GroupRoutine groupRoutine;

    // sender routine
    for (auto& user : mUserMap) {
        auto roomUser = user.second;

        groupRoutine.loopWithGroup([&](){
            Connection* connection = user.second->user->GetConnection();
            
            connection->waitWriteBuffer();
            connection->writeBuffer();
        });
    }

    bool clientReady = false;
    do {
        photon::thread_usleep(500 * 1000);
        clientReady = std::all_of(mUserMap.begin(), mUserMap.end(), [](const std::pair<int, RoomUser*>& pair){
            return pair.second->battleReady;
        });
    } while(!clientReady && mRun);

    LOG_INFO("room ", mId, " all client is ready");

    mStatusCode = game::GameStatusCode_Countdown;
    
    syncUser();

    photon::thread_sleep(mSetting.starting_countdown_second());

    for (const auto& entry : mUserMap) {
        entry.second->user->SetState(UserState::InGame);
    }

    std::random_device rd;

    // spawn routine
    groupRoutine.loopWithGroup([&]() {
        photon::thread_sleep(5);

        LOG_INFO("spawn rockfall");

        int count = rd() % 3 + 1;

        for (int iter = 0; iter < count; iter++) {
            auto rockfall = mWorldSimulator->spawnRockfall(simVec2(rd() % 18 + 2, 9), this->mSetting.rockfalll_damage());
            broadcastPacket(
                SharedPacketBuilder.addSpawnEntityPushMessage(game::EntityType_Rockfall, toEntityStatus(rockfall)).buildPacket()
            );
        }
    });

    // ping routine
    groupRoutine.loopWithGroup([&]() {
        for (auto& entry : mUserMap) {
            entry.second->user->GetPlayer()->startPingRoutine();
            sendPacket(entry.second->user, SharedPacketBuilder.addPingPushMessage(entry.second->user->GetPlayer()->getPing()).buildPacket());
        }

        photon::thread_sleep(3);
    });

    mStatusCode = game::GameStatusCode_Start;

    broadcastPacket(
        SharedPacketBuilder.addGameStatusPushMessage(game::GameStatusCode_Start, {}).buildPacket()
    );

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

    // sender routine을 포함한 routine 종료
    photon::thread* handle = photon::thread_create11([&](){
        groupRoutine.stopAndWait();
    });

    // 핑 패킷으로 sender routine lock 해제
    broadcastPacket(
        SharedPacketBuilder.addPingPushMessage(0).buildPacket()
    );

    mStatusCode = game::GameStatusCode_End;
    
    // routine 해제 대기
    photon::thread_join(photon::thread_enable_join(handle));

    // routine 해제가 완료되면 게임 종료 패킷 전송
    broadcastPacket(
        SharedPacketBuilder.addGameResultPushMessage(game::GameStatusCode::GameStatusCode_End, mWinnerId).buildPacket()
    );

    // sender routine이 종료됐으니 직접 전송
    for (const auto& entry : mUserMap) {
        entry.second->user->GetConnection()->writeBuffer();
    }

    return mWinnerId;
}

const game::RoomSetting& Room::getRoomSetting() {
    return mSetting;
}

const std::map<int, RoomUser*>& Room::getUsers() {
    return mUserMap;
}

Player* Room::getPlayer(User* user) {
    return mPlayerMap[user];
}

void Room::sendPacket(User* user, const PacketBuilder& packet) {
    const auto& buffer = packet.getIoBuffer();

    Connection* connection = user->GetConnection();

    while (!connection->bufferedWrite(buffer.begin(), buffer.size()))
        connection->writeBuffer();
}

void Room::broadcastPacket(const PacketBuilder& packet) {
    for (const auto& entry : mUserMap) {
        sendPacket(entry.second->user, packet);
    }
}

Room::~Room() {
    for (auto pair : mUserMap) {
        delete pair.second;
    }

    delete mWorldSimulator;
}