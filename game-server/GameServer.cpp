#include "GameServer.h"
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

enum {
    Attr_ClientAttr = 1,
};

struct ClientAttr {
    bool authenticated;
    std::string nickname;
    PacketBuilder packetBuilder;
    std::shared_ptr<Room> connectedRoom;
    int roomUserId;
    
    bool waitMigration;
    int migratedWorkerId;
};

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

GameServer::GameServer(photon::WorkPool* workPool, const std::string& bindIp, unsigned short port): mWorkPool(workPool), mGameServerCallback(new GameServerCallback(this)), mBindAddress(photon::net::IPAddr(bindIp.c_str()), port){
    mServer = new Server(workPool, mGameServerCallback);
}

MatchResult GameServer::matchRequest(const MatchMessage& message) {
    mMatchingQueue.push(message);
    return message.syncer->pop();
}

void GameServer::GameServerCallback::onConnect(const std::shared_ptr<Connection>& connection) {
    LOG_INFO("on connect client ", connection->id());
    auto clientAttr = new ClientAttr{false, "", {}, nullptr, -1};

    connection->setAttribute(Attr_ClientAttr, reinterpret_cast<uint64_t>(clientAttr), true);
}

void GameServer::GameServerCallback::onPacket(const std::shared_ptr<Connection>& connection, const game::Packet* packet, const char* buffer, int size) {

    auto clientAttr = reinterpret_cast<ClientAttr*>(connection->getAttribute(Attr_ClientAttr));

    if (clientAttr->waitMigration) {
        LOG_INFO("migrate to worker ", clientAttr->migratedWorkerId);
        mGameServer->mWorkPool->thread_migrate(photon::CURRENT, clientAttr->migratedWorkerId);
        clientAttr->waitMigration = false;
    }

    if (!clientAttr->authenticated) {
        auto messageTypes = packet->messages_type();
        if (messageTypes->size() != 1 || messageTypes->Get(0) != game::Payload_ConnectReq) {
            LOG_WARN("invalid packet inbound for authenticated");
            return;
        }
        
        auto connectReq = static_cast<const game::ConnectReq*>(packet->messages()->Get(0));
        
        mGameServer->mNicknameSetGaurd.lock();
        
        bool success = false;
        auto iter = mGameServer->mNicknameSet.find(connectReq->nickname()->str());
        if (iter == mGameServer->mNicknameSet.end()) {
            mGameServer->mNicknameSet.insert(connectReq->nickname()->str());
            success = true;
            clientAttr->authenticated = true;
            clientAttr->nickname = connectReq->nickname()->str();
            LOG_INFO("client ", connection->id(), " has nickname ", connectReq->nickname()->str().c_str());
        }
        mGameServer->mNicknameSetGaurd.unlock();

        clientAttr->packetBuilder.addConnectRepMessage(success);
        clientAttr->packetBuilder.sendPacketAndReset(connection.get());

        
        return;
    }

    if (clientAttr->connectedRoom == nullptr) {
        auto messageTypes = packet->messages_type();
        if (messageTypes->size() != 1 || messageTypes->Get(0) != game::Payload_MatchReq) {
            LOG_WARN("invalid packet inbound for matching");
            return;
        }

        auto matchReq = static_cast<const game::MatchReq*>(packet->messages()->Get(0));
        bool cancel = matchReq->cancel();
        
        // heap allocation is occurred, but it's called once in a while
        photon::thread_create11([=](){
            BlockingQueue<MatchResult> syncer;
            
            auto result = this->mGameServer->matchRequest({connection, cancel, &syncer});

            if (result.resultCode == game::MatchResultCode_Ok) {
                if (!connection->isConnected()) {
                    this->mGameServer->mWorkPool->thread_migrate(photon::CURRENT, result.workerId);
                    result.room->disconnectClient(result.userId, connection.get());
                    return;
                }

                clientAttr->connectedRoom = result.room;
                clientAttr->roomUserId = result.userId;
                
                // set thread migration info
                clientAttr->waitMigration = true;
                clientAttr->migratedWorkerId = result.workerId;

                const auto& userMap = result.room->getUsers();

                std::vector<Packet_User> packetUsers;
                for (auto entry : userMap) {
                    packetUsers.push_back({entry.second->userNickname, entry.first});
                }

                clientAttr->packetBuilder.addMatchRepMessage(result.resultCode, result.room->getRoomSetting(), packetUsers);
                clientAttr->packetBuilder.sendPacketAndReset(connection.get());

                return;

            }

            // create own packet builder to avoid race condition
            PacketBuilder builder;

            builder.addMatchRepMessage(result.resultCode, {}, {});
            builder.sendPacketAndReset(connection.get());
        });
        
        return;
    }
    
    clientAttr->connectedRoom->processClientPacket(clientAttr->roomUserId, packet);
}

void GameServer::GameServerCallback::onDisconnect(const std::shared_ptr<Connection>& connection) {
    LOG_INFO("on disconnect client ", connection->id());
    auto clientAttr = reinterpret_cast<ClientAttr*>(connection->getAttribute(Attr_ClientAttr));

    BlockingQueue<MatchResult> syncer;
    mGameServer->matchRequest({connection, true, &syncer});

    if (clientAttr->waitMigration) {
        LOG_INFO("migrate to worker ", clientAttr->migratedWorkerId);
        mGameServer->mWorkPool->thread_migrate(photon::CURRENT, clientAttr->migratedWorkerId);
        clientAttr->waitMigration = false;
    }

    if (clientAttr->connectedRoom != nullptr) {
        clientAttr->connectedRoom->disconnectClient(clientAttr->roomUserId, connection.get());
    }

    if (clientAttr->authenticated) {
        mGameServer->mNicknameSetGaurd.lock();
        mGameServer->mNicknameSet.erase(clientAttr->nickname);
        mGameServer->mNicknameSetGaurd.unlock();
    }
}

void startRoomSimulation(const std::shared_ptr<Room>& room, const std::shared_ptr<Connection>& connection1, const std::shared_ptr<Connection>& connection2) {

    int winnerId = room->startRoomSimulation();

    LOG_INFO("end room simulation");

    auto attr = reinterpret_cast<ClientAttr*>(connection1->getAttribute(Attr_ClientAttr));
    attr->connectedRoom = nullptr;

    attr = reinterpret_cast<ClientAttr*>(connection2->getAttribute(Attr_ClientAttr));
    attr->connectedRoom = nullptr;
}

int GameServer::start() {

    if (mServer->init(mBindAddress) != 0) {
        return -1;
    }

    mServer->start(false);

    auto defaultSetting = getDefaultRoomSetting();

    WorkerProfiler profiler(mWorkPool, defaultSetting.frame_rate());

    profiler.startProfiler();

    std::unordered_map<Connection*, MatchData> requestStatusMap;
    SequentialBuffer<MatchData*> unreadyBuffer(5000);
    SequentialBuffer<MatchData*> readyBuffer(3000);
    
    std::mt19937 rd;
    int roomSequence = 1;

    while(true) {
        photon::thread_usleep(500 * 1000);

        MatchMessage newMessage;
        auto now = std::chrono::system_clock::now();
        while(mMatchingQueue.try_pop(newMessage)) {

            auto requestIter = requestStatusMap.find(newMessage.connection.get());

            // filter invalid cancel request
            if (newMessage.cancel == (requestIter == requestStatusMap.end())) {
                newMessage.syncer->push(MatchResult{nullptr, 0, 0, game::MatchResultCode_Failed});
                continue;
            }

            if (newMessage.cancel) {
                LOG_INFO("cancel match ", newMessage.connection->id());

                requestIter->second.canceled = true;
                newMessage.syncer->push(MatchResult{nullptr, 0, 0, game::MatchResultCode_CancelComplete});
                
            } else {
                
                requestStatusMap[newMessage.connection.get()] = {newMessage.connection, std::chrono::system_clock::now() + std::chrono::seconds(2), newMessage.syncer, false};
                unreadyBuffer.pushBack(&requestStatusMap[newMessage.connection.get()]);

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

        if(readyBuffer.size() > 1) {
            LOG_INFO("start matching");
            uint32_t readyBufferSize = readyBuffer.size() - readyBuffer.size()%2;
            std::shuffle(readyBuffer.begin(), readyBuffer.end(), rd);

            for (int index = 0; index < readyBufferSize; index += 2) {
                
                auto user1 = readyBuffer[index];
                auto user2 = readyBuffer[index+1];

                if (user1->canceled || user2->canceled) {
                    if (user1->canceled) 
                        requestStatusMap.erase(user1->connection.get());
                    else
                        readyBuffer.pushBack(user1);

                    if (user2->canceled)
                        requestStatusMap.erase(user2->connection.get());
                    else
                        readyBuffer.pushBack(user2);
                    
                    continue;
                }

                auto connection1 = user1->connection;
                auto connection2 = user2->connection;

                LOG_INFO("match 2 client ", connection1->id(),", ", connection2->id());

                auto user1Attr = reinterpret_cast<ClientAttr*>(user1->connection->getAttribute(Attr_ClientAttr));
                auto user2Attr = reinterpret_cast<ClientAttr*>(user2->connection->getAttribute(Attr_ClientAttr));
                
                int selectedWorkerId = profiler.getAvailableWorkerNumber();
                
                if (selectedWorkerId == -1) {
                    user1->syncer->push(MatchResult{nullptr, -1, -1, game::MatchResultCode_Failed});
                    user2->syncer->push(MatchResult{nullptr, -1, -1, game::MatchResultCode_Failed});    

                    requestStatusMap.erase(connection1.get());
                    requestStatusMap.erase(connection2.get());
                    continue;
                }

                auto room = std::shared_ptr<Room>(new Room(roomSequence++, std::vector<UserInfo>({{user1Attr->nickname, connection1}, {user2Attr->nickname, connection2}}), defaultSetting));

                photon::thread_create11([=](){
                    LOG_INFO("start simulation room ", room->getRoomId(), ", with worker ", selectedWorkerId);
                    this->mWorkPool->thread_migrate(photon::CURRENT, selectedWorkerId);
                    startRoomSimulation(room, connection1, connection2);
                });

                user1->syncer->push(MatchResult{room, room->getUserId(connection1.get()), selectedWorkerId, game::MatchResultCode_Ok});
                user2->syncer->push(MatchResult{room, room->getUserId(connection2.get()), selectedWorkerId, game::MatchResultCode_Ok});

                requestStatusMap.erase(connection1.get());
                requestStatusMap.erase(connection2.get());
            }

            readyBuffer.eraseFront(readyBufferSize);
            LOG_INFO("match complete");
        } else if (readyBuffer.size() == 1 && readyBuffer[0]->canceled) {
            LOG_INFO("erase canceled request");
            auto matchData = readyBuffer[0];
            readyBuffer.eraseFront(1);
            requestStatusMap.erase(matchData->connection.get());
        }
    }
}

GameServer::~GameServer() {
    mServer->stop();
    delete mServer;
}