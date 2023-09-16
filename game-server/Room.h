#pragma once

#include "simulator/Simulator.h"
#include "net/Connection.h"
#include "net/protocol/protocol_generated.h"
#include "photon/common/lockfree_queue.h"
#include "photon/thread/workerpool.h"
#include "PacketBuilder.h"
#include "User.h"
#include <map>
#include <chrono>
#include <vector>
#include <memory>

struct RoomUser {
    bool battleReady;
    User* user;
};

game::Vec2 convertVec2(const simVec2& vec2);

game::EntityStatus toEntityStatus(IEntity* entity);

class Room : Simulator::EventCallback {
private:
    Simulator* mWorldSimulator;

    // Player Id -> User
    std::map<int, RoomUser*> mUserMap;

    // User -> Player
    std::map<User*, Player*> mPlayerMap;

    PacketBuilder mSharedPacketBuilder;

    bool mRun = true;
    game::GameStatusCode mStatusCode;
    game::RoomSetting mSetting;

    int mWinnerId = -1;
    int mId;

    const std::chrono::system_clock::time_point mCreationTime;

    void onPlayerHit(Player*);
    void syncUser();

    void sendPacket(User* user, const PacketBuilder& packet);

public:
    Room(int roomId, const std::vector<User*>& users, game::RoomSetting setting);
        
    void disconnectUser(User* user);
    void readyForBattle(User* user);

    int startRoomSimulation();

    int getRoomId();
    const game::RoomSetting& getRoomSetting();
    const std::map<int, RoomUser*>& getUsers();
    Player* getPlayer(User* user);

    void broadcastPacket(const PacketBuilder& packet);

    virtual void onPlayerContactMap(Player*, IMap*);
    virtual void onPlayerHitGunshot(Player*, GunShot*);
    virtual void onRockfallHitGunshot(Rockfall*, GunShot*);
    virtual void onHitRockfall(Player*, Rockfall*);
    virtual void onPickupHp(Player*, HpItem*);

    virtual ~Room();
};