#pragma once

#include <string>
#include "photon/net/socket.h"
#include "photon/common/io-alloc.h"
#include "photon/thread/thread11.h"
#include "photon/thread/workerpool.h"
#include "protocol/protocol_generated.h"
#include "Connection.h"
#include <memory>

using PacketLength = uint16_t;

constexpr uint16_t PacketLengthSize = sizeof(PacketLength);
constexpr unsigned char PROTOCOL_VERSION[] = {0, 1};
constexpr unsigned short PROTOCOL_VERSION_SCALAR = (PROTOCOL_VERSION[1] * 256) + PROTOCOL_VERSION[0];

class Server {
public:    
    class Callback {
    public:
        virtual void onConnect(const std::shared_ptr<Connection>& connection) = 0;
        virtual void onPacket(const std::shared_ptr<Connection>& connection, const game::Packet* packet, const char* buffer, int size) = 0;
        virtual void onDisconnect(const std::shared_ptr<Connection>& connection) = 0;
    };

    Server(photon::WorkPool* workPool,  Callback* callback) : mWorkerPool(workPool), mCallback(callback) {}
    int init(const photon::net::EndPoint& address);
    int init(const std::string& bindIp, unsigned short port);
    int start(bool block = true);
    void stop(bool gracefulShutdown = true);
private:
    photon::WorkPool* mWorkerPool = nullptr;
    photon::net::ISocketServer* mServer = nullptr;
    photon::net::EndPoint mEndpoint;
    Callback* mCallback = nullptr;
    std::atomic_bool mStart = false;

    int acceptHandler(photon::net::ISocketStream* stream);
};