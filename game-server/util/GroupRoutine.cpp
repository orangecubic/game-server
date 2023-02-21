#include "GroupRoutine.h"
#include "photon/thread/thread11.h"

void GroupRoutine::loopWithGroup(std::function<void()> logic) {

    // caused heap allocation
    auto thread = photon::thread_create11([this, logic](){
        while(this->mRun) {
            logic();
        }
    });

    mJoinHandles.push_back(photon::thread_enable_join(thread));
}

void GroupRoutine::stopAndWait() {
    mRun = false;

    for (auto handle : mJoinHandles) {
        photon::thread_join(handle);
    }
}