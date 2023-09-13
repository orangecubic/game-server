#pragma once

#include <vector>
#include <concepts>
#include <type_traits>
#include <cstring>

template <typename T>
concept ReadableStream = 
    requires(T t, void* buf, size_t size, int flag) {
        { t.recv(buf, size, flag) } -> std::same_as<ssize_t>;
    };

template<typename T>
concept QueueData = std::is_scalar<T>().value == true;

template <typename T> requires QueueData<T>
class SequentialBuffer {
private:
    static constexpr uint32_t DATA_SIZE = sizeof(T);
    uint32_t mRewindingSize;
    uint32_t mStartingIndex;
    T* mBuffer;

    uint32_t mCapacity;
    uint32_t mCount;
public:
    SequentialBuffer(unsigned int capacity) {
        mCount = mStartingIndex = 0;
        mCapacity = capacity;
        mRewindingSize = capacity/2;

        mBuffer = (T*)malloc(DATA_SIZE*mCapacity);
    }

    void pushBack(const T& data) {
        if (mCapacity == mStartingIndex + mCount) {
            uint32_t newCapacity = mCapacity*2;
            T* newBuffer = (T*)malloc(DATA_SIZE*newCapacity);

            std::memcpy(newBuffer, mBuffer+mStartingIndex, DATA_SIZE*mCount);

            mStartingIndex = 0;
            
            free(mBuffer);

            mBuffer = newBuffer;
            mCapacity = newCapacity;
            mRewindingSize = mCapacity/2;
        }
        mBuffer[mStartingIndex + mCount++] = data;
    }

    template <typename Stream> requires ReadableStream<Stream>
    int recv(Stream* stream) {
        int ret = stream->recv(end(), mCapacity - (mStartingIndex + mCount), 0);
        if (ret > 0) {
            mCount += ret;
        }
        return ret;
    }

    void eraseFront(uint32_t count) {
        assert(mCount >= count);
        mCount -= count;
        mStartingIndex += count;

        if (mStartingIndex >= mRewindingSize) {
            std::memcpy(mBuffer, mBuffer + mStartingIndex, DATA_SIZE*(mCapacity - mStartingIndex));
            mStartingIndex = 0;
        }

        if (mCount == 0) {
            mStartingIndex = 0;
        }
    }

    T* begin() {
        return mBuffer + mStartingIndex;
    }
    T* end() {
        return mBuffer + mStartingIndex + mCount;
    }   

    uint32_t getIndex(T* iterator) {
        return (uint32_t)(uint64_t(iterator) - uint64_t(begin()))/DATA_SIZE;
    }

    uint32_t size() {
        return mCount;
    }

    T& operator [](uint32_t index) {
        assert(index < mStartingIndex + mCount);
        return mBuffer[mStartingIndex + index];
    }

    ~SequentialBuffer() {
        free(mBuffer);
    }
};