#include "GroupRoutine.h"
#include "photon/thread/thread11.h"

void GroupRoutine::loopWithGroup(std::function<void()> logic) {

    // caused heap allocation
    auto thread = photon::thread_create11([this, logic](){
        while(this->mRun) {
            logic();
        }
    });

    mThreadHandles.push_back(thread);
}

void GroupRoutine::stopAndWait() {
    mRun = false;

    for (auto handle : mThreadHandles) {
        photon::thread_interrupt(handle);
        photon::thread_join(photon::thread_enable_join(handle));
    }
}