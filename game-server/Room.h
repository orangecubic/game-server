#pragma once

#include "simulator/Simulator.h"
#include "net/Connection.h"
#include "net/protocol/protocol_generated.h"
#include "photon/common/lockfree_queue.h"
#include "photon/thread/workerpool.h"
#include "PacketBuilder.h"
#include <chrono>
#include <vector>
#include <memory>

struct UserInfo {
    std::string nickname;
    std::shared_ptr<Connection> connection;
};

struct RoomUser {
    bool battleReady;
    IEntity* userEntity;
    std::shared_ptr<Connection> socketConnection;
    std::string userNickname;
    std::chrono::system_clock::time_point lastPingTime;
    std::chrono::system_clock::time_point lastShotTime;
    int currentPing;
    PacketBuilder packetBuilder;
};

class Room : Simulator::EventCallback {
private:
    Simulator* mWorldSimulator;
    std::unordered_map<int, RoomUser*> mUserList;
    std::unordered_map<Connection*, int> mConnectionMap;

    bool mRun = true;
    game::GameStatusCode mStatusCode;
    game::RoomSetting mSetting;

    int mWinnerId = -1;
    int mId;

    const std::chrono::system_clock::time_point mCreationTime;

    void onPlayerHit(Player*);
    void syncUser(RoomUser*);
public:
    Room(int roomId, const std::vector<UserInfo>& users, game::RoomSetting setting);
        
    void disconnectClient(int, Connection*);
    void processClientPacket(int userId, const game::Packet*);
    int startRoomSimulation();

    int getRoomId();
    int getUserId(Connection* connection);
    const game::RoomSetting& getRoomSetting();
    const std::unordered_map<int, RoomUser*>& getUsers();

    virtual void onPlayerContactMap(Player*, IMap*);
    virtual void onPlayerHitGunshot(Player*, GunShot*);
    virtual void onRockfallHitGunshot(Rockfall*, GunShot*);
    virtual void onHitRockfall(Player*, Rockfall*);
    virtual void onPickupHp(Player*, HpItem*);

    virtual ~Room();
};