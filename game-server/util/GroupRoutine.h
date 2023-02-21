#pragma once

#include <vector>
#include <functional>
#include "photon/thread/thread.h"

class GroupRoutine {
private:
    std::vector<photon::join_handle*> mJoinHandles;
    bool mRun = true;
public:
    void loopWithGroup(std::function<void()> logic);

    void stopAndWait();
};