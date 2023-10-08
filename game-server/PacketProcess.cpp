#include "PacketProcess.h"
#include "photon/common/alog.h"

#define USER_KEY 999

PacketProcess::PacketProcess(photon::WorkPool* workPool, MatchMaker* matchMaker) : mWorkPool(workPool), mMatchMaker(matchMaker) { }

void PacketProcess::onConnect(const std::shared_ptr<Connection>& connection) {
    connection->setAttribute(USER_KEY, reinterpret_cast<uint64_t>(new UserPtr(new User(mWorkPool, connection))), false);
}

void PacketProcess::onDisconnect(const std::shared_ptr<Connection>& connection) {
    LOG_INFO("on disconnect client ", connection->id());

    UserPtr& user = *reinterpret_cast<UserPtr*>(connection->getAttribute(USER_KEY));
    if (user == nullptr) {
        connection->close();
        return;
    }

    if (user->IsAuthenticated()) {
        photon::scoped_lock raiiLock(this->mNicknameGuard);
        mNicknameSet.erase(std::string(user->GetNickname()));
    }

    mMatchMaker->matchRequest(user, true);

    Room* room = user->GetRoom();
    if (room != nullptr) {
        user->migrateThreadIfNeed();
        room->disconnectUser(user.get());
    }

    delete &user;
}

void PacketProcess::onPacket(const std::shared_ptr<Connection>& connection, const game::Packet* packet, const char* buffer, int size) {
    
    auto payloadTypes = packet->messages_type();
    auto payloads = packet->messages();

    if (payloadTypes->size() != payloads->size()) {
        return;
    }

    UserPtr& user = *reinterpret_cast<UserPtr*>(connection->getAttribute(USER_KEY));
    if (user == nullptr) {
        connection->close();
        return;
    }
    
    user->migrateThreadIfNeed();

    flatbuffers::uoffset_t indexer = 0;
    for (auto payloadType : *payloadTypes) {

        if (!user->IsAvailableState((game::Payload)payloadType)) {
            LOG_WARN("not available state user: ", user->GetNickname().c_str(), ", packet: ", payloadType);
            continue;
        }

        switch (payloadType) {
        case game::Payload_ConnectReq:
            this->OnConnectReq(user, static_cast<const game::ConnectReq*>(payloads->Get(indexer)));
            return;
        case game::Payload_MatchReq:
            RoutinePerPacketMiddleware(user, static_cast<const game::MatchReq*>(payloads->Get(indexer)), OnMatchReq);
            return;
        case game::Payload_BattleReadyReq:
            this->OnBattleReadyReq(user, static_cast<const game::BattleReadyReq*>(payloads->Get(indexer)));
            break;
        case game::Payload_ChangePlayerStatusReq:
            this->OnChangePlayerStatusReq(user, static_cast<const game::ChangePlayerStatusReq*>(payloads->Get(indexer)));
            break;
        case game::Payload_PingAck:
            this->OnPingAck(user, static_cast<const game::PingAck*>(payloads->Get(indexer)));
            break;
        }
        
        indexer++;
    }
}

void PacketProcess::OnConnectReq(const UserPtr& user, const game::ConnectReq* packet) {

    photon::scoped_lock raiiLock(this->mNicknameGuard);
    
    bool success = false;
    auto iter = this->mNicknameSet.find(packet->nickname()->str());
    if (iter == this->mNicknameSet.end()) {
        this->mNicknameSet.insert(packet->nickname()->str());
        success = true;
        user->SetAuthenticated(packet->nickname()->str());
        user->SetState(UserState::Lobby);

        LOG_INFO("client ", user->GetConnection()->id(), " has nickname ", user->GetNickname().c_str());
    }

    SharedPacketBuilder.addConnectRepMessage(success).sendPacketAndReset(user);
}

void PacketProcess::OnMatchReq(const UserPtr& user, const game::MatchReq* packet) {

    user->SetState(UserState::Matching);
    bool cancel = packet->cancel();
    
    LOG_INFO("match request client", user->GetNickname().c_str());

    auto result = this->mMatchMaker->matchRequest(user, cancel);

    if (result.resultCode == game::MatchResultCode_Ok) {

        user->SetState(UserState::BattleWait);
        user->SetRoom(result.room, result.workerId, result.room->getPlayer(user.get()));

        // 매칭 완료 타이밍과 커넥션 종료가 겹치면 Room의 worker thread로 가서 유저 종료 처리
        if (!user->GetConnection()->isConnected()) {
            user->migrateThreadIfNeed();
            result.room->disconnectUser(user.get());
            return;
        }

        const auto& userMap = result.room->getUsers();

        std::vector<Packet_User> packetUsers;
        for (auto entry : userMap) {
            packetUsers.push_back({entry.second->user->GetNickname(), entry.first});
        }

        SharedPacketBuilder.addMatchRepMessage(result.resultCode, result.room->getRoomSetting(), packetUsers).sendPacketAndReset(user);

        return;

    }

    if (result.resultCode == game::MatchResultCode_Failed) {
        user->SetState(UserState::Lobby);
    }

    SharedPacketBuilder.addMatchRepMessage(result.resultCode, {}, {}).sendPacketAndReset(user);
}

void PacketProcess::OnBattleReadyReq(const UserPtr& user, const game::BattleReadyReq* packet) {
    Room* room = user->GetRoom();
    if(room != nullptr)
        room->readyForBattle(user.get());
}

void PacketProcess::OnChangePlayerStatusReq(const UserPtr& user, const game::ChangePlayerStatusReq* packet) {

    Player* player = user->GetPlayer();
    if (player == nullptr)
        return;

    int actionCount = 0;
    std::array<uint8_t, 5> allowedActions;

    for (auto action : *packet->action()) {
        switch (action) {
        case game::EntityAction_Left:
            player->setMovementStatus(PlayerMovementStatus_Left);
            allowedActions[actionCount++] = action;
            break;
        case game::EntityAction_Right:
            player->setMovementStatus(PlayerMovementStatus_Right);
            allowedActions[actionCount++] = action;
            break;
        case game::EntityAction_Stop:
            player->setMovementStatus(PlayerMovementStatus_Stop);
            allowedActions[actionCount++] = action;
            break;
        case game::EntityAction_Jump:
            if (player->jump(simVec2(player->getLinearVelocity().x, user->GetRoom()->getRoomSetting().player_jump_velocity()))) {
                allowedActions[actionCount++] = action;
            }
            break;
        case game::EntityAction_Shot:

            if (player->shot()) {
                allowedActions[actionCount++] = action;
            }

            break;
        }
    }

    user->GetRoom()->broadcastPacket(
        SharedPacketBuilder.addChangeEntityStatusPushMessage(allowedActions.begin(), actionCount, toEntityStatus(player)).buildPacket()
    );
}

void PacketProcess::OnPingAck(const UserPtr& user, const game::PingAck* packet) {
    Player* player = user->GetPlayer();
    if (player == nullptr)
        return;
    player->endPingRoutine();
}
