#pragma once

#include "net/protocol/protocol_generated.h"
#include "net/Connection.h"
#include "PacketBuilder.h"
#include "simulator/Player.h"

enum UserState {
    Lobby,
    Matching,
    BattleWait,
    InGame,
};

class Room;
class User {
public:
    User(const std::shared_ptr<Connection>& connection);

    bool IsAuthenticated();
    std::string_view GetNickname();
    
    Connection* GetConnection();
    int GetWorkerId();
    Room* GetRoom();
    Player* GetPlayer();
    PacketBuilder* GetPacketBuilder();

    void SetAuthenticated(std::string_view nickname);
    void SetRoom(const std::shared_ptr<Room>& room, int workerIndex, Player* player);
    void SetState(UserState state);
    bool IsAvailableState(game::Payload packet);

    void migrateThreadIfNeed();
private:
    bool mAuthenticated;
    std::shared_ptr<Connection> mConnection;
    std::string mNickname;
    std::shared_ptr<Room> mRoom;
    Player* mGamePlayer;

    int mRoomWorker = -1;    
};

using UserPtr = std::shared_ptr<User>;