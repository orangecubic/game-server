#include "WorkerProfiler.h"
#include "photon/thread/thread11.h"
#include "photon/common/alog.h"
#include <random>
#include <algorithm>

WorkerProfiler::WorkerProfiler(photon::WorkPool* workPool, int frameRate) : mWorkPool(workPool), mServerFrameRate(frameRate), mAvailableWorkers(workPool->get_vcpu_num()) {
    
}

void WorkerProfiler::profilerMain(int number) {
    this->mWorkPool->thread_migrate(photon::CURRENT, number);

    int sleepTime = (int) (1000.0/float(mServerFrameRate) * 0.8);

    SequentialBuffer<uint64_t> lastDeltaBuffer(200);

    float criticalRatio = 1.3;
    uint64_t samplingCount = 30;
    uint64_t sumDelta = 0;

    while(true) {
        auto start = std::chrono::system_clock::now();
        photon::thread_usleep(sleepTime * 1000);
        auto end = std::chrono::system_clock::now();

        auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>((end - start)).count();

        lastDeltaBuffer.pushBack(waitTime);
        if (lastDeltaBuffer.size() > samplingCount) {
            sumDelta -= *lastDeltaBuffer.begin();
            lastDeltaBuffer.eraseFront(1);
        }

        sumDelta += waitTime;
        this->mAvailableWorkers[number] = (sumDelta / samplingCount < uint64_t(float(sleepTime)*criticalRatio));
    }
}

void WorkerProfiler::startProfiler() {

    for (int num = 0; num < mWorkPool->get_vcpu_num(); num++) {
        photon::thread_create11([=](){
            this->profilerMain(num);
        });
    }
}

int WorkerProfiler::getAvailableWorkerNumber() {
    std::random_device rd;

    int retryCount = mWorkPool->get_vcpu_num();
    int workerNumber = -1;
    
    while(retryCount-- != 0) {
        workerNumber = rd() % mWorkPool->get_vcpu_num();


        if (mAvailableWorkers[workerNumber]) {
            return workerNumber;
        }
    }

    return -1;
}

WorkerProfiler::~WorkerProfiler() {

}
