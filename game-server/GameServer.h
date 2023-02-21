#pragma once

#include "net/Server.h"
#include "net/protocol/protocol_generated.h"
#include "net/protocol/game_packet_generated.h"
#include "simulator/Simulator.h"
#include "photon/thread/thread.h"
#include "photon/common/lockfree_queue.h"
#include "util/BlockingQueue.h"
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
    std::shared_ptr<Connection> connection;
    std::chrono::system_clock::time_point timeout;
    BlockingQueue<MatchResult>* syncer;
    bool canceled;
};

struct MatchMessage {
    std::shared_ptr<Connection> connection;
    bool cancel;
    BlockingQueue<MatchResult>* syncer;

};

class GameServer {
private:
    Server* mServer;
    photon::net::EndPoint mBindAddress;

    photon::WorkPool* mWorkPool;
    
    std::set<std::string> mNicknameSet;
    photon::mutex mNicknameSetGaurd;
    
    // matching variable
    BlockingQueue<MatchMessage> mMatchingQueue;
    // matching variable

    class GameServerCallback : public Server::Callback{
    private:
        GameServer* mGameServer;
    public:
        GameServerCallback(GameServer* gameServer) : mGameServer(gameServer) {}
        virtual void onConnect(const std::shared_ptr<Connection>& connection) override;
        virtual void onPacket(const std::shared_ptr<Connection>& connection, const game::Packet* packet, const char* buffer, int size) override;
        virtual void onDisconnect(const std::shared_ptr<Connection>& connection) override;
    };

    GameServerCallback* mGameServerCallback;

    MatchResult matchRequest(const MatchMessage&);

public:

    GameServer(photon::WorkPool* workPool, const std::string& bindIp, unsigned short port);

    int start();

    virtual ~GameServer();
};