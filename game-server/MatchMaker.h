#pragma once

#include "net/Server.h"
#include "net/protocol/protocol_generated.h"
#include "net/protocol/game_packet_generated.h"
#include "simulator/Simulator.h"
#include "photon/thread/thread.h"
#include "photon/common/lockfree_queue.h"
#include "util/BlockingQueue.h"
#include "User.h"
#include "Room.h"
#include <set>
#include <list>
#include <random>

struct MatchResult {
    std::shared_ptr<Room> room;
    int userId;
    int workerId;
    game::MatchResultCode resultCode;
};

struct MatchData {
    UserPtr user;
    std::chrono::system_clock::time_point timeout;
    BlockingQueue<MatchResult>* syncer;
    bool canceled;
};

struct MatchMessage {
    UserPtr user;
    bool cancel;
    BlockingQueue<MatchResult>* syncer;
};

class MatchMaker {
private:
    
    // matching variable
    BlockingQueue<MatchMessage> mMatchingQueue;
    // matching variable

    photon::WorkPool* mWorkPool;
public:
    MatchMaker(photon::WorkPool* workPool);
    MatchResult matchRequest(const UserPtr& user, bool cancel);
    int start();
};