#include "User.h"

User::User(photon::WorkPool* workPool, const std::shared_ptr<Connection>& connection) : mWorkPool(workPool), mConnection(connection) { }

bool User::IsAuthenticated() {
    return mAuthenticated;
}

const std::string& User::GetNickname() {
    return mNickname;
}

Connection* User::GetConnection() {
    return mConnection.get();
}

Room* User::GetRoom() {
    return mRoom.get();
}

Player* User::GetPlayer() {
    return mGamePlayer;
}

void User::SetAuthenticated(std::string_view nickname) {
    mAuthenticated = true;
    mNickname = nickname;
}

void User::SetRoom(const std::shared_ptr<Room>& room, int workerIndex, Player* player) {
    mRoom = room;
    mRoomWorker = workerIndex;
    mGamePlayer = player;
}

void User::SetState(UserState state) {
    mState = state;
}

void User::migrateThreadIfNeed() {
    if (mRoomWorker != -1) {
        mWorkPool->thread_migrate(photon::CURRENT, mRoomWorker);
        mRoomWorker = -1;
    }
}

bool User::IsAvailableState(game::Payload packet) {

    switch(mState) {
    case UserState::Unauthenticated:
        switch(packet) {
        case game::Payload::Payload_ConnectReq:
            return true;
        default:
            return false;    
        }
    case UserState::Lobby:
        switch(packet) {
        case game::Payload::Payload_MatchReq:
            return true;
        default:
            return false;    
        }
    case UserState::Matching:
        switch(packet) {
        case game::Payload::Payload_MatchReq:
            return true;
        default:
            return false;    
        }
    case UserState::BattleWait:
        switch(packet) {
        case game::Payload::Payload_BattleReadyReq:
            return true;
        default:
            return false;    
        }
    case UserState::InGame:
        switch(packet) {
        case game::Payload_ChangePlayerStatusReq:
        case game::Payload::Payload_PingAck:
            return true;
        default:
            return false;    
        }
    }

}