#include "Server.h"
#include "photon/common/alog.h"
#include "../util/SequentialBuffer.h"

constexpr uint16_t BUFFER_SIZE = 4096;

int Server::init(const std::string& bindIp, unsigned short port) {
    return init(photon::net::EndPoint(photon::net::IPAddr(bindIp.c_str()), port));
}

int Server::start(bool block) {

    if (this->mServer == nullptr) {
        LOG_ERRNO_RETURN(0, -1, "cannot start uninitialized server");
    }

    if (this->mServer->bind(this->mEndpoint.port, this->mEndpoint.addr) != 0) {
        LOG_ERRNO_RETURN(0, -1, "failed to bind server");
    }


    int reuse = 1;
    unsigned short rcvBufSize = BUFFER_SIZE;
    this->mStart = true;
    this->mServer->set_handler(photon::net::ISocketServer::Handler(this, &Server::acceptHandler));
    this->mServer->setsockopt(SOL_SOCKET, SO_RCVBUF, &rcvBufSize);
    this->mServer->setsockopt(SOL_SOCKET, SO_REUSEADDR, &reuse);

    this->mServer->listen();
    return this->mServer->start_loop(block);
}

int Server::init(const photon::net::EndPoint& address) {
    if (this->mStart) {
        LOG_ERRNO_RETURN(0, -1, "cannot reinitialize started server");
    }

    this->mServer = photon::net::new_tcp_socket_server();
    //this->mServer = photon::net::new_iouring_tcp_server();
    if (this->mServer == nullptr) {
        LOG_ERRNO_RETURN(0, -1, "failed to create socket server");
    }
    
    this->mEndpoint = address;
    return 0;
}

void Server::stop(bool gracefulShutdown) {
    this->mServer->terminate();
    if (gracefulShutdown) {

    }
}

int Server::acceptHandler(photon::net::ISocketStream* stream) {
    this->mWorkerPool->thread_migrate();

    std::shared_ptr<Connection> connection(new Connection(stream));

    flatbuffers::FlatBufferBuilder defaultPacketBuilder;
    SequentialBuffer<char> buffer(BUFFER_SIZE);
    
    this->mCallback->onConnect(connection);

    DEFER(connection->close());
    DEFER(this->mCallback->onDisconnect(connection));

    while (true) {

        while (buffer.size() < PacketLengthSize) {
            int ret = buffer.recv(stream);
            if (ret <= 0) {
                return 0;
            }
        }

        PacketLength packetLength;
        memcpy(&packetLength, buffer.begin(), PacketLengthSize);

        if (packetLength > BUFFER_SIZE / 2) {
            LOG_WARN("invalid packet size inbound");
            return 0;
        }

        buffer.eraseFront(PacketLengthSize);

        while (buffer.size() < packetLength) {
            int ret = buffer.recv(stream);
            if (ret <= 0) {
                return 0;
            }
        }

        auto packet = game::GetPacket(buffer.begin());

        flatbuffers::Verifier verifier((const uint8_t*)(buffer.begin()), packetLength);
        
        if(!game::VerifyPacketBuffer(verifier)) {
            LOG_WARN("corrupted packet inbound");
            return 0;
        } else if (packet->version() != PROTOCOL_VERSION_SCALAR) {
            LOG_INFO("packet version mismatch for ", connection->id(), " '", PROTOCOL_VERSION_SCALAR, ", ", packet->version(), "'");

            if (defaultPacketBuilder.GetSize() == 0) {
                auto offset = game::CreatePacketDirect(defaultPacketBuilder, PROTOCOL_VERSION_SCALAR, {}, {});
                defaultPacketBuilder.Finish(offset);
            }

            unsigned short packetSize = defaultPacketBuilder.GetSize();
            iovec ioBuf[] = {
                {
                    &packetSize,
                    2
                },
                {
                    defaultPacketBuilder.GetBufferPointer(),
                    defaultPacketBuilder.GetSize(),
                }
            };
            stream->writev(ioBuf, 2);

        } else {
            this->mCallback->onPacket(connection, packet, buffer.begin(), packetLength+PacketLengthSize);
        }
        buffer.eraseFront(packetLength);

    }
    return 0;
}