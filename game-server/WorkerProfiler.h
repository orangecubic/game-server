#pragma once

#include "photon/thread/thread.h"
#include "photon/thread/workerpool.h"
#include "util/SequentialBuffer.h"
#include "Room.h"
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <atomic>

class WorkerProfiler {
private:
    photon::WorkPool* mWorkPool;
    std::vector<std::atomic_bool> mAvailableWorkers;

    int mServerFrameRate;

    void profilerMain(int number);
public:
    WorkerProfiler(photon::WorkPool* workPool, int frameRate);

    void startProfiler();

    int getAvailableWorkerNumber();

    ~WorkerProfiler();
};