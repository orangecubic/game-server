#pragma once

#include "net/Connection.h"
#include "net/protocol/game_packet_generated.h"
#include "net/protocol/protocol_generated.h"
#include "photon/thread/thread.h"

struct Packet_User {
    std::string_view nickname;
    int id;
};

class PacketBuilder {
private:
    flatbuffers::FlatBufferBuilder mBuilder;
    std::vector<uint8_t> mPayloadTypeList;
    std::vector<flatbuffers::Offset<void>> mPayloadOffsetList;
    photon::condition_variable mPacketMonitor;

    Connection* mConnection;
public:

    PacketBuilder(Connection* connection);    

    void addConnectRepMessage(bool success);
    void addMatchRepMessage(game::MatchResultCode code, const game::RoomSetting& roomSetting, const std::vector<Packet_User>& userList);

    void addPingPushMessage(int delay);
    void addGameStatusPushMessage(game::GameStatusCode code, const std::vector<game::UserStatus>& userStatus);
    void addGameResultPushMessage(game::GameStatusCode code, int winnerId);
    void addSpawnEntityPushMessage(game::EntityType type, const game::EntityStatus& status);

    void addCollisionEventPushMessage(int collisionCode, const game::EntityStatus& entityAStatus, const game::EntityStatus& entityBStatus);
    void addChangeEntityStatusPushMessage(const uint8_t* actions, int actionCount, const game::EntityStatus& entityBStatus);

    int sendPacketAndReset();

    void waitPacket();
    void releaseAll();
    const std::vector<iovec>& getPacketBuffer();

    
};