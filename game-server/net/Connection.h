#pragma once

#include "photon/net/socket.h"
#include "photon/thread/thread11.h"
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
    int readv(const iovec* vec, int count);

    int write(void* buf, size_t size);
    int writev(const iovec* vec, int count);
    
    bool bufferedWrite(const iovec* vec, int count);
    int writeBuffer();
    void waitWriteBuffer();

    void close();
    void setAttribute(int key, uint64_t value, bool autoDelete);
    uint64_t getAttribute(int key);
    bool isConnected();

    virtual ~Connection();
private:
    bool mIsConnected = true;

    
    photon::net::ISocketStream* mStream;
    photon::condition_variable mWriteBufferMonitor;
    std::array<unsigned char, 65536> mWriteBuffer;
    int mWriteBufferSize = 0;

    std::unordered_map<int, ConnAttr> mAttributes;
};