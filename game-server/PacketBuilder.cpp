#include "PacketBuilder.h"
#include "net/Server.h"
#include <algorithm>
#include <vector>

thread_local PacketBuilder* tlsPacketBuilder = new PacketBuilder;

PacketBuilder& _sharedPacketBuilder() {
    return *tlsPacketBuilder;
}

PacketBuilder& PacketBuilder::addConnectRepMessage(bool success) {
    auto offset = game::CreateConnectRep(mBuilder, success);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_ConnectRep);
    return *this;
}

PacketBuilder& PacketBuilder::addMatchRepMessage(game::MatchResultCode code, const game::RoomSetting& roomSetting, const std::vector<Packet_User>& userList) {

    int index = 0;
    std::vector<flatbuffers::Offset<game::UserInfo>> offsetVector(userList.size());
    for (auto& userInfo : userList) {
        offsetVector[index++] = game::CreateUserInfo(mBuilder, userInfo.id, mBuilder.CreateString(userInfo.nickname));
    }

    auto offset = game::CreateMatchRepDirect(mBuilder, code, &roomSetting, &offsetVector);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_MatchRep);
    return *this;
}

PacketBuilder& PacketBuilder::addPingPushMessage(int delay) {
    auto offset = game::CreatePingPush(mBuilder, delay);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_PingPush);
    return *this;
}

PacketBuilder& PacketBuilder::addGameStatusPushMessage(game::GameStatusCode code, const std::vector<game::UserStatus>& userStatus) {
    auto offset = game::CreateGameStatusPush(mBuilder, code, mBuilder.CreateVectorOfStructs(userStatus));

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_GameStatusPush);
    return *this;
}

PacketBuilder& PacketBuilder::addGameResultPushMessage(game::GameStatusCode code, int winnerId) {
    auto offset = game::CreateGameStatusPush(mBuilder, code, 0, winnerId);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_GameStatusPush);
    return *this;
}

PacketBuilder& PacketBuilder::addSpawnEntityPushMessage(game::EntityType type, const game::EntityStatus& status) {
    auto offset = game::CreateSpawnEntityPush(mBuilder, type, &status);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_SpawnEntityPush);
    return *this;
}

PacketBuilder& PacketBuilder::addCollisionEventPushMessage(int collisionCode, const game::EntityStatus& entityAStatus, const game::EntityStatus& entityBStatus) {
    auto offset = game::CreateCollisionEventPush(mBuilder, collisionCode, &entityAStatus, &entityBStatus);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_CollisionEventPush);
    return *this;
}

PacketBuilder& PacketBuilder::addChangeEntityStatusPushMessage(const uint8_t* actions, int actionCount, const game::EntityStatus& entityStatus) {
    auto offset = game::CreateChangeEntityStatusPush(mBuilder, mBuilder.CreateVector(actions, actionCount), &entityStatus);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_ChangeEntityStatusPush);
    return *this;
}
int PacketBuilder::sendPacketAndReset(const UserPtr& user) {
    return sendPacketAndReset(user.get());
}

int PacketBuilder::sendPacketAndReset(User* user) {
    if (mPayloadOffsetList.empty()) {
        return -1;
    }
    
    buildPacket();

    const auto& buffer = getIoBuffer();
    int result = user->GetConnection()->writev(buffer.begin(), buffer.size());

    resetIoBuffer();

    return result;
}

PacketBuilder& PacketBuilder::buildPacket() {

    if (mPayloadOffsetList.empty()) {
        return *this;
    }

    auto packetOffset = game::CreatePacket(mBuilder, PROTOCOL_VERSION_SCALAR, mBuilder.CreateVector(mPayloadTypeList), mBuilder.CreateVector(mPayloadOffsetList));
    mBuilder.Finish(packetOffset);

    short packetSize = mBuilder.GetSize();
    this->mSendIoVector[0] = {
        &packetSize,
        2
    };
    this->mSendIoVector[1] = {
        mBuilder.GetBufferPointer(),
        mBuilder.GetSize()
    };

    mPayloadOffsetList.clear();
    mPayloadTypeList.clear();

    return *this;
}

const std::array<iovec, 2>& PacketBuilder::getIoBuffer() const {
    return this->mSendIoVector;
}

PacketBuilder& PacketBuilder::resetIoBuffer() {
    mBuilder.Clear();

    return *this;
}

