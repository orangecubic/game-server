#pragma once

#include "net/Connection.h"
#include "net/protocol/game_packet_generated.h"
#include "net/protocol/protocol_generated.h"
#include "photon/thread/thread.h"
#include "User.h"

struct Packet_User {
    std::string_view nickname;
    int id;
};

class PacketBuilder {
private:
    flatbuffers::FlatBufferBuilder mBuilder;
    std::vector<uint8_t> mPayloadTypeList;
    std::vector<flatbuffers::Offset<void>> mPayloadOffsetList;

    std::array<iovec, 2> mSendIoVector;
public:
    PacketBuilder& addConnectRepMessage(bool success);
    PacketBuilder& addMatchRepMessage(game::MatchResultCode code, const game::RoomSetting& roomSetting, const std::vector<Packet_User>& userList);

    PacketBuilder& addPingPushMessage(int delay);
    PacketBuilder& addGameStatusPushMessage(game::GameStatusCode code, const std::vector<game::UserStatus>& userStatus);
    PacketBuilder& addGameResultPushMessage(game::GameStatusCode code, int winnerId);
    PacketBuilder& addSpawnEntityPushMessage(game::EntityType type, const game::EntityStatus& status);

    PacketBuilder& addCollisionEventPushMessage(int collisionCode, const game::EntityStatus& entityAStatus, const game::EntityStatus& entityBStatus);
    PacketBuilder& addChangeEntityStatusPushMessage(const uint8_t* actions, int actionCount, const game::EntityStatus& entityBStatus);

    int sendPacketAndReset(const UserPtr& user);
    int sendPacketAndReset(User* user);

    PacketBuilder& buildPacket();
    const std::array<iovec, 2>& getIoBuffer() const;
    PacketBuilder& resetIoBuffer();
};

PacketBuilder& _sharedPacketBuilder();

#define SharedPacketBuilder _sharedPacketBuilder().resetIoBuffer()