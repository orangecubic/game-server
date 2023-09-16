#pragma once

#include "net/protocol/protocol_generated.h"
#include "photon/thread/workerpool.h"
#include "net/Connection.h"
#include "simulator/Player.h"

enum UserState {
    Unauthenticated,
    Lobby,
    Matching,
    BattleWait,
    InGame,
};

class Room;
class User {
public:
    User(photon::WorkPool* workPool, const std::shared_ptr<Connection>& connection);

    bool IsAuthenticated();
    const std::string& GetNickname();
    
    Connection* GetConnection();\
    Room* GetRoom();
    Player* GetPlayer();

    void SetAuthenticated(std::string_view nickname);
    void SetRoom(const std::shared_ptr<Room>& room, int workerIndex, Player* player);
    void SetState(UserState state);
    bool IsAvailableState(game::Payload packet);

    void migrateThreadIfNeed();
private:

    UserState mState = UserState::Unauthenticated;

    bool mAuthenticated = false;

    photon::WorkPool* mWorkPool;
    std::shared_ptr<Connection> mConnection = nullptr;
    std::string mNickname;
    std::shared_ptr<Room> mRoom = nullptr;
    Player* mGamePlayer;

    int mRoomWorker = -1;    
};

using UserPtr = std::shared_ptr<User>;