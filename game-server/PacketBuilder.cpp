#include "PacketBuilder.h"
#include "net/Server.h"
#include <algorithm>
#include <vector>

Packetbuilder::PacketBuilder(Connection* connection) : mConnection(connection) { }

void PacketBuilder::addConnectRepMessage(bool success) {
    auto offset = game::CreateConnectRep(mBuilder, success);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_ConnectRep);
    mPacketMonitor.notify_one();
}

void PacketBuilder::addMatchRepMessage(game::MatchResultCode code, const game::RoomSetting& roomSetting, const std::vector<Packet_User>& userList) {

    int index = 0;
    std::vector<flatbuffers::Offset<game::UserInfo>> offsetVector(userList.size());
    for (auto& userInfo : userList) {
        offsetVector[index++] = game::CreateUserInfo(mBuilder, userInfo.id, mBuilder.CreateString(userInfo.nickname));
    }

    auto offset = game::CreateMatchRepDirect(mBuilder, code, &roomSetting, &offsetVector);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_MatchRep);
    mPacketMonitor.notify_one();
}

void PacketBuilder::addPingPushMessage(int delay) {
    auto offset = game::CreatePingPush(mBuilder, delay);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_PingPush);
    mPacketMonitor.notify_one();
}

void PacketBuilder::addGameStatusPushMessage(game::GameStatusCode code, const std::vector<game::UserStatus>& userStatus) {
    auto offset = game::CreateGameStatusPush(mBuilder, code, mBuilder.CreateVectorOfStructs(userStatus));

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_GameStatusPush);

    mPacketMonitor.notify_one();
}

void PacketBuilder::addGameResultPushMessage(game::GameStatusCode code, int winnerId) {
    auto offset = game::CreateGameStatusPush(mBuilder, code, 0, winnerId);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_GameStatusPush);

    mPacketMonitor.notify_one();
}

void PacketBuilder::addSpawnEntityPushMessage(game::EntityType type, const game::EntityStatus& status) {
    auto offset = game::CreateSpawnEntityPush(mBuilder, type, &status);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_SpawnEntityPush);

    mPacketMonitor.notify_one();
}

void PacketBuilder::addCollisionEventPushMessage(int collisionCode, const game::EntityStatus& entityAStatus, const game::EntityStatus& entityBStatus) {
    auto offset = game::CreateCollisionEventPush(mBuilder, collisionCode, &entityAStatus, &entityBStatus);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_CollisionEventPush);

    mPacketMonitor.notify_one();
}

void PacketBuilder::addChangeEntityStatusPushMessage(const uint8_t* actions, int actionCount, const game::EntityStatus& entityStatus) {
    auto offset = game::CreateChangeEntityStatusPush(mBuilder, mBuilder.CreateVector(actions, actionCount), &entityStatus);

    mPayloadOffsetList.push_back(offset.Union());
    mPayloadTypeList.push_back(game::Payload_ChangeEntityStatusPush);

    mPacketMonitor.notify_one();
}

void PacketBuilder::waitPacket() {
    if(mBuilder.GetSize() != 0) {
        return;
    }
    mPacketMonitor.wait_no_lock();
}

void PacketBuilder::releaseAll() {
    mPacketMonitor.notify_all();
}

int PacketBuilder::sendPacketAndReset() {
    if (mPayloadOffsetList.empty()) {
        return -1;
    }
    
    auto packetOffset = game::CreatePacket(mBuilder, PROTOCOL_VERSION_SCALAR, mBuilder.CreateVector(mPayloadTypeList), mBuilder.CreateVector(mPayloadOffsetList));
    mBuilder.Finish(packetOffset);

    short packetSize = mBuilder.GetSize();
    iovec sendVec[2] = {
        {
            &packetSize,
            2
        },
        {
            mBuilder.GetBufferPointer(),
            mBuilder.GetSize()
        }
    };
    int result = this->mConnection->writev(sendVec, 2);

    mPayloadOffsetList.clear();
    mPayloadTypeList.clear();
    mBuilder.Clear();

    return result;
}
