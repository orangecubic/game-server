#include "PacketProcess.h"
#include "photon/common/alog.h"

#define USER_KEY 999

void PacketProcess::onConnect(const std::shared_ptr<Connection>& connection) {
    connection->setAttribute(USER_KEY, reinterpret_cast<uint64_t>(new UserPtr(new User(connection))), false);
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

    BlockingQueue<MatchResult> syncer;
    mMatchMaker->matchRequest({user, true, &syncer});

    Room* room = user->GetRoom();
    if (room != nullptr) {
        this->mWorkPool->thread_migrate(photon::CURRENT, user->GetWorkerId());
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
            continue;
        }

        switch (payloadType) {
        case game::Payload_ConnectReq:
            this->OnConnectReq(user, static_cast<const game::ConnectReq*>(payloads->Get(indexer++)));
            return;
        case game::Payload_MatchReq:
            RoutinePerPacketMiddleware(user, static_cast<const game::MatchReq*>(payloads->Get(indexer++)), OnMatchReq);
            return;
        case game::Payload_BattleReadyReq:
            RoutinePerPacketMiddleware(user, static_cast<const game::BattleReadyReq*>(payloads->Get(indexer++)), OnBattleReadyReq);
            break;
        case game::Payload_ChangePlayerStatusReq:
            RoutinePerPacketMiddleware(user, static_cast<const game::ChangePlayerStatusReq*>(payloads->Get(indexer++)), OnChangePlayerStatusReq);
            break;
        case game::Payload_PingAck:
            RoutinePerPacketMiddleware(user, static_cast<const game::PingAck*>(payloads->Get(indexer++)), OnPingAck);
            break;
        }
        
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

        LOG_INFO("client ", user->GetConnection()->id(), " has nickname ", user->GetNickname());
    }

    user->GetPacketBuilder()->addConnectRepMessage(success);
    user->GetPacketBuilder()->sendPacketAndReset();
}

void PacketProcess::OnMatchReq(const UserPtr& user, const game::MatchReq* packet) {

    user->SetState(UserState::Matching);
    bool cancel = packet->cancel();
        
    auto result = this->mMatchMaker->matchRequest(user, cancel);

    if (result.resultCode == game::MatchResultCode_Ok) {
        if (!user->GetConnection()->isConnected()) {
            this->mWorkPool->thread_migrate(photon::CURRENT, result.workerId);
            result.room->disconnectUser(user.get());
            return;
        }

        user->SetState(UserState::BattleWait);
        user->SetRoom(result.room, result.workerId, result.room->getUsers()[result.userId]);

        const auto& userMap = result.room->getUsers();

        std::vector<Packet_User> packetUsers;
        for (auto entry : userMap) {
            packetUsers.push_back({entry.second->user->GetNickname(), entry.first});
        }

        user->GetPacketBuilder()->addMatchRepMessage(result.resultCode, result.room->getRoomSetting(), packetUsers);
        user->GetPacketBuilder()->sendPacketAndReset();

        return;

    }

    if (result.resultCode == game::MatchResultCode_Failed) {
        user->SetState(UserState::Lobby);
    }

    // create own packet builder to avoid race condition
    PacketBuilder builder(user->GetConnection());

    builder.addMatchRepMessage(result.resultCode, {}, {});
    builder.sendPacketAndReset();

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
            auto now = std::chrono::system_clock::now();

            if ((now - std::chrono::milliseconds(player->getLastShotTime())).time_since_epoch().count() >= user->GetRoom()->getRoomSetting().player_shot_cooltime()) {
                // mWorldSimulator->spawnPlayerGunShot(player, mSetting.gunshot_damage());
                allowedActions[actionCount++] = action;
                player->setLastShotTime();
            }

            break;
        }
    }
}

void PacketProcess::OnPingAck(const UserPtr& user, const game::PingAck* packet) {
    Player* player = user->GetPlayer();
    if (player == nullptr)
        return;
    player->setLastPingTime();
}
