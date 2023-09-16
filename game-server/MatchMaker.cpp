#include "MatchMaker.h"
#include "photon/photon.h"
#include "net/protocol/protocol_generated.h"
#include "util/SequentialBuffer.h"
#include "photon/common/message-channel.h"
#include "photon/common/alog.h"
#include "PacketBuilder.h"
#include "WorkerProfiler.h"
#include "Room.h"
#include <cstring>
#include <cmath>
#include <queue>
#include <memory>

inline game::RoomSetting getDefaultRoomSetting() {
    return game::RoomSetting(
        30,                 // frame rate
        5,                  // countdown seconds before start
        -10,                // gravity
        5,                  // player hp
        2,                  // gunshot damage
        1,                  // rockfall damage
        1,                  // shot cooltime
        17,                 // player movement force
        22,                 // player jump velocity
        1                   // hp item point
    );
}

MatchMaker::MatchMaker(photon::WorkPool* workPool) : mWorkPool(workPool) { }

MatchResult MatchMaker::matchRequest(const UserPtr& user, bool cancel) {
    BlockingQueue<MatchResult> syncer;
    mMatchingQueue.push({user, cancel, &syncer});
    return syncer.pop();
}

void startRoomSimulation(const std::shared_ptr<Room>& room, const UserPtr& user1, const UserPtr& user2) {

    int winnerId = room->startRoomSimulation();

    LOG_INFO("end room simulation");

    user1->SetRoom(nullptr, -1, nullptr);
    user2->SetRoom(nullptr, -1, nullptr);
    
    user1->SetState(UserState::Lobby);
    user2->SetState(UserState::Lobby);
}

int MatchMaker::start() {

    auto defaultSetting = getDefaultRoomSetting();

    WorkerProfiler profiler(mWorkPool, defaultSetting.frame_rate());

    profiler.startProfiler();

    std::map<User*, MatchData> requestStatusMap;
    SequentialBuffer<MatchData*> unreadyBuffer(5000);
    SequentialBuffer<MatchData*> readyBuffer(3000);
    
    std::mt19937 rd;
    int roomSequence = 1;

    while(true) {
        photon::thread_usleep(500 * 1000);

        MatchMessage newMessage;
        auto now = std::chrono::system_clock::now();
        while(mMatchingQueue.try_pop(newMessage)) {

            auto requestIter = requestStatusMap.find(newMessage.user.get());

            // filter invalid cancel request
            if (newMessage.cancel == (requestIter == requestStatusMap.end())) {
                newMessage.syncer->push(MatchResult{nullptr, 0, 0, game::MatchResultCode_Failed});
                continue;
            }

            if (newMessage.cancel) {
                LOG_INFO("cancel match ", newMessage.user->GetConnection()->id());

                requestIter->second.canceled = true;
                newMessage.syncer->push(MatchResult{nullptr, 0, 0, game::MatchResultCode_CancelComplete});
                
            } else {
                
                requestStatusMap[newMessage.user.get()] = {newMessage.user, std::chrono::system_clock::now() + std::chrono::seconds(2), newMessage.syncer, false};
                unreadyBuffer.pushBack(&requestStatusMap[newMessage.user.get()]);

            }
        }

        uint32_t readyIndex = 0;
        for (; readyIndex < unreadyBuffer.size(); readyIndex++) {
            auto matchData = unreadyBuffer[readyIndex];

            if (matchData->timeout <= now) {
                readyBuffer.pushBack(matchData);
            } else {
                break;
            }
        }

        if(readyIndex > 0) {
            unreadyBuffer.eraseFront(readyIndex);
        }

        if(readyBuffer.size() < 2) {

            if (readyBuffer.size() == 1 && readyBuffer[0]->canceled) {
                LOG_INFO("erase canceled request");
                
                auto matchData = readyBuffer[0];

                readyBuffer.eraseFront(1);
                requestStatusMap.erase(matchData->user.get());
            }
            continue;
        }

        LOG_INFO("start matching");
        uint32_t readyBufferSize = readyBuffer.size() - readyBuffer.size()%2;
        std::shuffle(readyBuffer.begin(), readyBuffer.end(), rd);

        for (int index = 0; index < readyBufferSize; index += 2) {
            
            auto request1 = readyBuffer[index];
            auto request2 = readyBuffer[index+1];

            if (request1->canceled || request2->canceled) {
                if (request1->canceled) 
                    requestStatusMap.erase(request1->user.get());
                else
                    readyBuffer.pushBack(request1);

                if (request2->canceled)
                    requestStatusMap.erase(request2->user.get());
                else
                    readyBuffer.pushBack(request2);
                
                continue;
            }

            auto user1 = request1->user;
            auto user2 = request2->user;

            LOG_INFO("match 2 client ", user1->GetConnection()->id(),", ", user2->GetConnection()->id());

            int selectedWorkerId = profiler.getAvailableWorkerNumber();
            
            if (selectedWorkerId == -1) {
                request1->syncer->push(MatchResult{nullptr, -1, -1, game::MatchResultCode_Failed});
                request2->syncer->push(MatchResult{nullptr, -1, -1, game::MatchResultCode_Failed});    

                requestStatusMap.erase(user1.get());
                requestStatusMap.erase(user2.get());
                continue;
            }

            auto room = std::shared_ptr<Room>(new Room(roomSequence++, std::vector<User*>({user1.get(), user2.get()}), defaultSetting));

            photon::thread_create11([=](){
                LOG_INFO("start simulation room ", room->getRoomId(), ", with worker ", selectedWorkerId);
                this->mWorkPool->thread_migrate(photon::CURRENT, selectedWorkerId);
                startRoomSimulation(room, user1, user2);
            });

            for (const auto& entry : room->getUsers()) {
                if (entry.second->user == user1.get()) {
                    request1->syncer->push(MatchResult{room, entry.first, selectedWorkerId, game::MatchResultCode_Ok});
                } else if (entry.second->user == user2.get()) {
                    request2->syncer->push(MatchResult{room, entry.first, selectedWorkerId, game::MatchResultCode_Ok});
                }
            }

            requestStatusMap.erase(user1.get());
            requestStatusMap.erase(user2.get());
        }

        readyBuffer.eraseFront(readyBufferSize);
        LOG_INFO("match complete");
    }
}