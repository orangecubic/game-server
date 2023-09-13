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

class Room : Simulator::EventCallback {
private:
    Simulator* mWorldSimulator;
    std::map<int, RoomUser*> mUserMap;

    bool mRun = true;
    game::GameStatusCode mStatusCode;
    game::RoomSetting mSetting;

    int mWinnerId = -1;
    int mId;

    const std::chrono::system_clock::time_point mCreationTime;

    void onPlayerHit(Player*);
    void syncUser(RoomUser*);
public:
    Room(int roomId, const std::vector<User*>& users, game::RoomSetting setting);
        
    void disconnectUser(User* user);
    void readyForBattle(User* user);

    int startRoomSimulation();

    int getRoomId();
    const game::RoomSetting& getRoomSetting();
    const std::map<int, RoomUser*>& getUsers();

    virtual void onPlayerContactMap(Player*, IMap*);
    virtual void onPlayerHitGunshot(Player*, GunShot*);
    virtual void onRockfallHitGunshot(Rockfall*, GunShot*);
    virtual void onHitRockfall(Player*, Rockfall*);
    virtual void onPickupHp(Player*, HpItem*);

    virtual ~Room();
};