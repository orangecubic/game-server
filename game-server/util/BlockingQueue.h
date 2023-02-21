#pragma once

#include "photon/thread/thread.h"
#include "photon/common/lockfree_queue.h"
#include <queue>

template <typename T>
class BlockingQueue {
private:
    photon::mutex mMutex;
    photon::condition_variable mCond;
    std::queue<T> mQueue;
public:

    T pop() {
        photon::scoped_lock lock(mMutex);

        while(mQueue.empty()) {
            mCond.wait(lock);
        }

        auto ret = mQueue.front();
        mQueue.pop();
        return ret;
    }

    void push(const T& data) {
        photon::scoped_lock lock(mMutex);
        mQueue.push(data);

        mCond.notify_one();
    }

    bool try_pop(T& data) {
        photon::scoped_lock lock(mMutex);
        if (mQueue.empty())
            return false;

        data = mQueue.front();
        mQueue.pop();
        return true;
    }
};