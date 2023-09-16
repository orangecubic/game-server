#pragma once

#include <string>
#include <netinet/tcp.h>
#include "photon/net/socket.h"
#include "photon/common/io-alloc.h"
#include "photon/thread/thread11.h"
#include "photon/thread/workerpool.h"
#include "protocol/protocol_generated.h"
#include "Connection.h"
#include <memory>

using PacketLength = uint16_t;

// 패킷 사이즈 헤더의 길이를 2바이트로 정의
constexpr uint16_t PacketLengthSize = sizeof(PacketLength);

// 프로토콜 버전 0.1
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
    void stop();
private:

    // 클라이언트 루틴이 실행될 worker의 pool
    photon::WorkPool* mWorkerPool = nullptr;

    // 서버
    photon::net::ISocketServer* mServer = nullptr;

    // 서버 바인드 주소 정보
    photon::net::EndPoint mEndpoint;

    // 서버 callback
    Callback* mCallback = nullptr;

    std::atomic_bool mStart = false;

    int acceptHandler(photon::net::ISocketStream* stream);
};