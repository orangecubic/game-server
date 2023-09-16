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

bool Connection::bufferedWrite(const iovec* iovecList, int count) {

    int totalWriteSize = 0;
    for (int index = 0; index < count; index++)
        totalWriteSize += iovecList[index].iov_len;

    if (mWriteBufferSize + totalWriteSize > mWriteBuffer.size())
        return false;

    for (int index = 0; index < count; index++) {

        const iovec& vec = iovecList[index];

        std::memcpy(&mWriteBuffer[mWriteBufferSize], vec.iov_base, vec.iov_len);

        mWriteBufferSize += vec.iov_len;
    }

    mWriteBufferMonitor.notify_one();

    return true;
}

int Connection::writeBuffer() {

    if (mWriteBufferSize == 0)
        return 0;

    int result = write(mWriteBuffer.begin(), mWriteBufferSize);

    mWriteBufferSize = 0;

    return result;
}

void Connection::waitWriteBuffer() {
    mWriteBufferMonitor.wait_no_lock();
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