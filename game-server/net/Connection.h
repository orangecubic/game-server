#pragma once

#include "photon/net/socket.h"
#include <unordered_map>
#include <atomic>

struct ConnAttr {
    bool autoDelete;
    uint64_t pointer;
};

class Connection {
public:
    Connection(photon::net::ISocketStream* stream) : mStream(stream) {}

    int id();
    int read(void* buf, size_t size);
    int write(void* buf, size_t size);
    int readv(const iovec* vec, int count);
    int writev(const iovec* vec, int count);
    void close();
    void setAttribute(int key, uint64_t value, bool autoDelete);
    uint64_t getAttribute(int key);
    bool isConnected();

    virtual ~Connection();
private:
    bool mIsConnected = true;
    photon::net::ISocketStream* mStream;
    std::unordered_map<int, ConnAttr> mAttributes;
};