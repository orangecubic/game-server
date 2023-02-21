#include "Connection.h"

int Connection::id() {
    if (!mIsConnected)
        return -1;
    return this->mStream->get_underlay_fd();
}

int Connection::read(void* buf, size_t size) {
    if (!mIsConnected) 
        return -1;
    return this->mStream->read(buf, size);
}

int Connection::write(void* buf, size_t size) {
    if (!mIsConnected) 
        return -1;
    return this->mStream->write(buf, size);
}

int Connection::readv(const iovec* vec, int count) {
    if (!mIsConnected) 
        return -1;
    return this->mStream->readv(vec, count);
}

int Connection::writev(const iovec* vec, int count) {
    if (!mIsConnected) 
        return -1;
    return this->mStream->writev(vec, count);
}

void Connection::close() {
    mIsConnected = false;
    this->mStream->close();
}

void Connection::setAttribute(int key, uint64_t value, bool autoDelete) {
    this->mAttributes[key] = ConnAttr{autoDelete, value};
}

uint64_t Connection::getAttribute(int key) {
    auto iter = mAttributes.find(key);
    if (iter == mAttributes.end()) {
        return 0;
    }

    return iter->second.pointer;
}

bool Connection::isConnected() {
    return this->mIsConnected;
}

Connection::~Connection() {
    for (auto pair: mAttributes) {
        if (pair.second.autoDelete) {
            delete reinterpret_cast<void*>(pair.second.pointer);
        }
    }
}