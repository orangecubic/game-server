#pragma once

#include "net/protocol/game_packet_generated.h"
#include "net/protocol/protocol_generated.h"
#include "net/Connection.h"
#include "User.h"
#include "photon/thread/thread11.h"
#include "MatchMaker.h"

class PacketProcess : public Server::Callback {
public:

    PacketProcess(MatchMaker* matchMaker);

    virtual void onConnect(const std::shared_ptr<Connection>& connection) override;
    virtual void onPacket(const std::shared_ptr<Connection>& connection, const game::Packet* packet, const char* buffer, int size) override;
    virtual void onDisconnect(const std::shared_ptr<Connection>& connection) override;
private:
    MatchMaker* mMatchMaker;
    std::set<std::string> mNicknameSet;    
    photon::mutex mNicknameGuard;
    photon::WorkPool* mWorkPool;

    template <typename T, typename Handler>
    void RoutinePerPacketMiddleware(const UserPtr& user, const T* packet, const Handler& handler)
    {
        photon::thread_create11([=]() {
            handler(connection, packet);
        });
    }

    void OnConnectReq(const UserPtr& user, const game::ConnectReq* packet);
    void OnMatchReq(const UserPtr& user, const game::MatchReq* packet);
    void OnBattleReadyReq(const UserPtr& user, const game::BattleReadyReq* packet);
    void OnChangePlayerStatusReq(const UserPtr& user, const game::ChangePlayerStatusReq* packet);
    void OnPingAck(const UserPtr& user, const game::PingAck* packet);
};