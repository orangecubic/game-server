// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_GAMEPACKET_GAME_H_
#define FLATBUFFERS_GENERATED_GAMEPACKET_GAME_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 22 &&
              FLATBUFFERS_VERSION_MINOR == 12 &&
              FLATBUFFERS_VERSION_REVISION == 6,
             "Non-compatible flatbuffers version included");

namespace game {

struct UserInfo;
struct UserInfoBuilder;

struct UserStatus;

struct RoomSetting;

struct ConnectReq;
struct ConnectReqBuilder;

struct ConnectRep;
struct ConnectRepBuilder;

struct MatchReq;
struct MatchReqBuilder;

struct MatchRep;
struct MatchRepBuilder;

struct BattleReadyReq;
struct BattleReadyReqBuilder;

struct GameStatusPush;
struct GameStatusPushBuilder;

struct Vec2;

struct EntityStatus;

struct SpawnEntityPush;
struct SpawnEntityPushBuilder;

struct ChangePlayerStatusReq;
struct ChangePlayerStatusReqBuilder;

struct CollisionEventPush;
struct CollisionEventPushBuilder;

struct ChangeEntityStatusPush;
struct ChangeEntityStatusPushBuilder;

struct ChangePlayerStatusPush;
struct ChangePlayerStatusPushBuilder;

struct PingPush;
struct PingPushBuilder;

struct PingAck;
struct PingAckBuilder;

enum MatchResultCode : int8_t {
  MatchResultCode_Ok = 0,
  MatchResultCode_NoOpponent = 1,
  MatchResultCode_Failed = 2,
  MatchResultCode_CancelComplete = 3,
  MatchResultCode_MIN = MatchResultCode_Ok,
  MatchResultCode_MAX = MatchResultCode_CancelComplete
};

inline const MatchResultCode (&EnumValuesMatchResultCode())[4] {
  static const MatchResultCode values[] = {
    MatchResultCode_Ok,
    MatchResultCode_NoOpponent,
    MatchResultCode_Failed,
    MatchResultCode_CancelComplete
  };
  return values;
}

inline const char * const *EnumNamesMatchResultCode() {
  static const char * const names[5] = {
    "Ok",
    "NoOpponent",
    "Failed",
    "CancelComplete",
    nullptr
  };
  return names;
}

inline const char *EnumNameMatchResultCode(MatchResultCode e) {
  if (flatbuffers::IsOutRange(e, MatchResultCode_Ok, MatchResultCode_CancelComplete)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesMatchResultCode()[index];
}

enum GameStatusCode : uint8_t {
  GameStatusCode_Wait = 0,
  GameStatusCode_Countdown = 1,
  GameStatusCode_Start = 2,
  GameStatusCode_DropWait = 3,
  GameStatusCode_End = 4,
  GameStatusCode_MIN = GameStatusCode_Wait,
  GameStatusCode_MAX = GameStatusCode_End
};

inline const GameStatusCode (&EnumValuesGameStatusCode())[5] {
  static const GameStatusCode values[] = {
    GameStatusCode_Wait,
    GameStatusCode_Countdown,
    GameStatusCode_Start,
    GameStatusCode_DropWait,
    GameStatusCode_End
  };
  return values;
}

inline const char * const *EnumNamesGameStatusCode() {
  static const char * const names[6] = {
    "Wait",
    "Countdown",
    "Start",
    "DropWait",
    "End",
    nullptr
  };
  return names;
}

inline const char *EnumNameGameStatusCode(GameStatusCode e) {
  if (flatbuffers::IsOutRange(e, GameStatusCode_Wait, GameStatusCode_End)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesGameStatusCode()[index];
}

enum EntityType : uint8_t {
  EntityType_Map = 1,
  EntityType_Player = 2,
  EntityType_Rockfall = 4,
  EntityType_PlayerGunShot = 8,
  EntityType_FieldItem = 16,
  EntityType_MIN = EntityType_Map,
  EntityType_MAX = EntityType_FieldItem
};

inline const EntityType (&EnumValuesEntityType())[5] {
  static const EntityType values[] = {
    EntityType_Map,
    EntityType_Player,
    EntityType_Rockfall,
    EntityType_PlayerGunShot,
    EntityType_FieldItem
  };
  return values;
}

inline const char * const *EnumNamesEntityType() {
  static const char * const names[17] = {
    "Map",
    "Player",
    "",
    "Rockfall",
    "",
    "",
    "",
    "PlayerGunShot",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "FieldItem",
    nullptr
  };
  return names;
}

inline const char *EnumNameEntityType(EntityType e) {
  if (flatbuffers::IsOutRange(e, EntityType_Map, EntityType_FieldItem)) return "";
  const size_t index = static_cast<size_t>(e) - static_cast<size_t>(EntityType_Map);
  return EnumNamesEntityType()[index];
}

enum EntityAction : uint8_t {
  EntityAction_Left = 0,
  EntityAction_Right = 1,
  EntityAction_Stop = 2,
  EntityAction_Jump = 3,
  EntityAction_Shot = 4,
  EntityAction_MIN = EntityAction_Left,
  EntityAction_MAX = EntityAction_Shot
};

inline const EntityAction (&EnumValuesEntityAction())[5] {
  static const EntityAction values[] = {
    EntityAction_Left,
    EntityAction_Right,
    EntityAction_Stop,
    EntityAction_Jump,
    EntityAction_Shot
  };
  return values;
}

inline const char * const *EnumNamesEntityAction() {
  static const char * const names[6] = {
    "Left",
    "Right",
    "Stop",
    "Jump",
    "Shot",
    nullptr
  };
  return names;
}

inline const char *EnumNameEntityAction(EntityAction e) {
  if (flatbuffers::IsOutRange(e, EntityAction_Left, EntityAction_Shot)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesEntityAction()[index];
}

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(2) UserStatus FLATBUFFERS_FINAL_CLASS {
 private:
  uint8_t id_;
  uint8_t me_;
  uint8_t connected_;
  int8_t padding0__;
  uint16_t ping_;

 public:
  UserStatus()
      : id_(0),
        me_(0),
        connected_(0),
        padding0__(0),
        ping_(0) {
    (void)padding0__;
  }
  UserStatus(uint8_t _id, bool _me, bool _connected, uint16_t _ping)
      : id_(flatbuffers::EndianScalar(_id)),
        me_(flatbuffers::EndianScalar(static_cast<uint8_t>(_me))),
        connected_(flatbuffers::EndianScalar(static_cast<uint8_t>(_connected))),
        padding0__(0),
        ping_(flatbuffers::EndianScalar(_ping)) {
    (void)padding0__;
  }
  uint8_t id() const {
    return flatbuffers::EndianScalar(id_);
  }
  bool me() const {
    return flatbuffers::EndianScalar(me_) != 0;
  }
  bool connected() const {
    return flatbuffers::EndianScalar(connected_) != 0;
  }
  uint16_t ping() const {
    return flatbuffers::EndianScalar(ping_);
  }
};
FLATBUFFERS_STRUCT_END(UserStatus, 6);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) RoomSetting FLATBUFFERS_FINAL_CLASS {
 private:
  uint8_t frame_rate_;
  uint8_t starting_countdown_second_;
  int8_t world_gravity_;
  uint8_t initial_player_hp_;
  uint8_t gunshot_damage_;
  uint8_t rockfalll_damage_;
  uint8_t player_shot_cooltime_;
  int8_t padding0__;
  float player_ground_movement_force_;
  float player_jump_velocity_;
  uint8_t hp_item_value_;
  int8_t padding1__;  int16_t padding2__;

 public:
  RoomSetting()
      : frame_rate_(0),
        starting_countdown_second_(0),
        world_gravity_(0),
        initial_player_hp_(0),
        gunshot_damage_(0),
        rockfalll_damage_(0),
        player_shot_cooltime_(0),
        padding0__(0),
        player_ground_movement_force_(0),
        player_jump_velocity_(0),
        hp_item_value_(0),
        padding1__(0),
        padding2__(0) {
    (void)padding0__;
    (void)padding1__;
    (void)padding2__;
  }
  RoomSetting(uint8_t _frame_rate, uint8_t _starting_countdown_second, int8_t _world_gravity, uint8_t _initial_player_hp, uint8_t _gunshot_damage, uint8_t _rockfalll_damage, uint8_t _player_shot_cooltime, float _player_ground_movement_force, float _player_jump_velocity, uint8_t _hp_item_value)
      : frame_rate_(flatbuffers::EndianScalar(_frame_rate)),
        starting_countdown_second_(flatbuffers::EndianScalar(_starting_countdown_second)),
        world_gravity_(flatbuffers::EndianScalar(_world_gravity)),
        initial_player_hp_(flatbuffers::EndianScalar(_initial_player_hp)),
        gunshot_damage_(flatbuffers::EndianScalar(_gunshot_damage)),
        rockfalll_damage_(flatbuffers::EndianScalar(_rockfalll_damage)),
        player_shot_cooltime_(flatbuffers::EndianScalar(_player_shot_cooltime)),
        padding0__(0),
        player_ground_movement_force_(flatbuffers::EndianScalar(_player_ground_movement_force)),
        player_jump_velocity_(flatbuffers::EndianScalar(_player_jump_velocity)),
        hp_item_value_(flatbuffers::EndianScalar(_hp_item_value)),
        padding1__(0),
        padding2__(0) {
    (void)padding0__;
    (void)padding1__;
    (void)padding2__;
  }
  uint8_t frame_rate() const {
    return flatbuffers::EndianScalar(frame_rate_);
  }
  uint8_t starting_countdown_second() const {
    return flatbuffers::EndianScalar(starting_countdown_second_);
  }
  int8_t world_gravity() const {
    return flatbuffers::EndianScalar(world_gravity_);
  }
  uint8_t initial_player_hp() const {
    return flatbuffers::EndianScalar(initial_player_hp_);
  }
  uint8_t gunshot_damage() const {
    return flatbuffers::EndianScalar(gunshot_damage_);
  }
  uint8_t rockfalll_damage() const {
    return flatbuffers::EndianScalar(rockfalll_damage_);
  }
  uint8_t player_shot_cooltime() const {
    return flatbuffers::EndianScalar(player_shot_cooltime_);
  }
  float player_ground_movement_force() const {
    return flatbuffers::EndianScalar(player_ground_movement_force_);
  }
  float player_jump_velocity() const {
    return flatbuffers::EndianScalar(player_jump_velocity_);
  }
  uint8_t hp_item_value() const {
    return flatbuffers::EndianScalar(hp_item_value_);
  }
};
FLATBUFFERS_STRUCT_END(RoomSetting, 20);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Vec2 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;

 public:
  Vec2()
      : x_(0),
        y_(0) {
  }
  Vec2(float _x, float _y)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
};
FLATBUFFERS_STRUCT_END(Vec2, 8);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) EntityStatus FLATBUFFERS_FINAL_CLASS {
 private:
  int32_t id_;
  int8_t hp_;
  int8_t padding0__;  int16_t padding1__;
  game::Vec2 position_;
  game::Vec2 linear_velocity_;

 public:
  EntityStatus()
      : id_(0),
        hp_(0),
        padding0__(0),
        padding1__(0),
        position_(),
        linear_velocity_() {
    (void)padding0__;
    (void)padding1__;
  }
  EntityStatus(int32_t _id, int8_t _hp, const game::Vec2 &_position, const game::Vec2 &_linear_velocity)
      : id_(flatbuffers::EndianScalar(_id)),
        hp_(flatbuffers::EndianScalar(_hp)),
        padding0__(0),
        padding1__(0),
        position_(_position),
        linear_velocity_(_linear_velocity) {
    (void)padding0__;
    (void)padding1__;
  }
  int32_t id() const {
    return flatbuffers::EndianScalar(id_);
  }
  int8_t hp() const {
    return flatbuffers::EndianScalar(hp_);
  }
  const game::Vec2 &position() const {
    return position_;
  }
  const game::Vec2 &linear_velocity() const {
    return linear_velocity_;
  }
};
FLATBUFFERS_STRUCT_END(EntityStatus, 24);

struct UserInfo FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef UserInfoBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_NICKNAME = 6
  };
  uint8_t id() const {
    return GetField<uint8_t>(VT_ID, 0);
  }
  const flatbuffers::String *nickname() const {
    return GetPointer<const flatbuffers::String *>(VT_NICKNAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_ID, 1) &&
           VerifyOffset(verifier, VT_NICKNAME) &&
           verifier.VerifyString(nickname()) &&
           verifier.EndTable();
  }
};

struct UserInfoBuilder {
  typedef UserInfo Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint8_t id) {
    fbb_.AddElement<uint8_t>(UserInfo::VT_ID, id, 0);
  }
  void add_nickname(flatbuffers::Offset<flatbuffers::String> nickname) {
    fbb_.AddOffset(UserInfo::VT_NICKNAME, nickname);
  }
  explicit UserInfoBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<UserInfo> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<UserInfo>(end);
    return o;
  }
};

inline flatbuffers::Offset<UserInfo> CreateUserInfo(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint8_t id = 0,
    flatbuffers::Offset<flatbuffers::String> nickname = 0) {
  UserInfoBuilder builder_(_fbb);
  builder_.add_nickname(nickname);
  builder_.add_id(id);
  return builder_.Finish();
}

inline flatbuffers::Offset<UserInfo> CreateUserInfoDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint8_t id = 0,
    const char *nickname = nullptr) {
  auto nickname__ = nickname ? _fbb.CreateString(nickname) : 0;
  return game::CreateUserInfo(
      _fbb,
      id,
      nickname__);
}

struct ConnectReq FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ConnectReqBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NICKNAME = 4
  };
  const flatbuffers::String *nickname() const {
    return GetPointer<const flatbuffers::String *>(VT_NICKNAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NICKNAME) &&
           verifier.VerifyString(nickname()) &&
           verifier.EndTable();
  }
};

struct ConnectReqBuilder {
  typedef ConnectReq Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_nickname(flatbuffers::Offset<flatbuffers::String> nickname) {
    fbb_.AddOffset(ConnectReq::VT_NICKNAME, nickname);
  }
  explicit ConnectReqBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<ConnectReq> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ConnectReq>(end);
    return o;
  }
};

inline flatbuffers::Offset<ConnectReq> CreateConnectReq(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> nickname = 0) {
  ConnectReqBuilder builder_(_fbb);
  builder_.add_nickname(nickname);
  return builder_.Finish();
}

inline flatbuffers::Offset<ConnectReq> CreateConnectReqDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *nickname = nullptr) {
  auto nickname__ = nickname ? _fbb.CreateString(nickname) : 0;
  return game::CreateConnectReq(
      _fbb,
      nickname__);
}

struct ConnectRep FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ConnectRepBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SUCCESS = 4
  };
  bool success() const {
    return GetField<uint8_t>(VT_SUCCESS, 0) != 0;
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_SUCCESS, 1) &&
           verifier.EndTable();
  }
};

struct ConnectRepBuilder {
  typedef ConnectRep Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_success(bool success) {
    fbb_.AddElement<uint8_t>(ConnectRep::VT_SUCCESS, static_cast<uint8_t>(success), 0);
  }
  explicit ConnectRepBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<ConnectRep> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ConnectRep>(end);
    return o;
  }
};

inline flatbuffers::Offset<ConnectRep> CreateConnectRep(
    flatbuffers::FlatBufferBuilder &_fbb,
    bool success = false) {
  ConnectRepBuilder builder_(_fbb);
  builder_.add_success(success);
  return builder_.Finish();
}

struct MatchReq FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef MatchReqBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_CANCEL = 4
  };
  bool cancel() const {
    return GetField<uint8_t>(VT_CANCEL, 0) != 0;
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_CANCEL, 1) &&
           verifier.EndTable();
  }
};

struct MatchReqBuilder {
  typedef MatchReq Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_cancel(bool cancel) {
    fbb_.AddElement<uint8_t>(MatchReq::VT_CANCEL, static_cast<uint8_t>(cancel), 0);
  }
  explicit MatchReqBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<MatchReq> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<MatchReq>(end);
    return o;
  }
};

inline flatbuffers::Offset<MatchReq> CreateMatchReq(
    flatbuffers::FlatBufferBuilder &_fbb,
    bool cancel = false) {
  MatchReqBuilder builder_(_fbb);
  builder_.add_cancel(cancel);
  return builder_.Finish();
}

struct MatchRep FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef MatchRepBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_CODE = 4,
    VT_SETTING = 6,
    VT_USERS = 8
  };
  game::MatchResultCode code() const {
    return static_cast<game::MatchResultCode>(GetField<int8_t>(VT_CODE, 0));
  }
  const game::RoomSetting *setting() const {
    return GetStruct<const game::RoomSetting *>(VT_SETTING);
  }
  const flatbuffers::Vector<flatbuffers::Offset<game::UserInfo>> *users() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<game::UserInfo>> *>(VT_USERS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_CODE, 1) &&
           VerifyField<game::RoomSetting>(verifier, VT_SETTING, 4) &&
           VerifyOffset(verifier, VT_USERS) &&
           verifier.VerifyVector(users()) &&
           verifier.VerifyVectorOfTables(users()) &&
           verifier.EndTable();
  }
};

struct MatchRepBuilder {
  typedef MatchRep Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_code(game::MatchResultCode code) {
    fbb_.AddElement<int8_t>(MatchRep::VT_CODE, static_cast<int8_t>(code), 0);
  }
  void add_setting(const game::RoomSetting *setting) {
    fbb_.AddStruct(MatchRep::VT_SETTING, setting);
  }
  void add_users(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<game::UserInfo>>> users) {
    fbb_.AddOffset(MatchRep::VT_USERS, users);
  }
  explicit MatchRepBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<MatchRep> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<MatchRep>(end);
    return o;
  }
};

inline flatbuffers::Offset<MatchRep> CreateMatchRep(
    flatbuffers::FlatBufferBuilder &_fbb,
    game::MatchResultCode code = game::MatchResultCode_Ok,
    const game::RoomSetting *setting = nullptr,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<game::UserInfo>>> users = 0) {
  MatchRepBuilder builder_(_fbb);
  builder_.add_users(users);
  builder_.add_setting(setting);
  builder_.add_code(code);
  return builder_.Finish();
}

inline flatbuffers::Offset<MatchRep> CreateMatchRepDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    game::MatchResultCode code = game::MatchResultCode_Ok,
    const game::RoomSetting *setting = nullptr,
    const std::vector<flatbuffers::Offset<game::UserInfo>> *users = nullptr) {
  auto users__ = users ? _fbb.CreateVector<flatbuffers::Offset<game::UserInfo>>(*users) : 0;
  return game::CreateMatchRep(
      _fbb,
      code,
      setting,
      users__);
}

struct BattleReadyReq FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef BattleReadyReqBuilder Builder;
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct BattleReadyReqBuilder {
  typedef BattleReadyReq Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  explicit BattleReadyReqBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<BattleReadyReq> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<BattleReadyReq>(end);
    return o;
  }
};

inline flatbuffers::Offset<BattleReadyReq> CreateBattleReadyReq(
    flatbuffers::FlatBufferBuilder &_fbb) {
  BattleReadyReqBuilder builder_(_fbb);
  return builder_.Finish();
}

struct GameStatusPush FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef GameStatusPushBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_STATUS_CODE = 4,
    VT_USERS = 6,
    VT_WINNER_ID = 8
  };
  game::GameStatusCode status_code() const {
    return static_cast<game::GameStatusCode>(GetField<uint8_t>(VT_STATUS_CODE, 0));
  }
  const flatbuffers::Vector<const game::UserStatus *> *users() const {
    return GetPointer<const flatbuffers::Vector<const game::UserStatus *> *>(VT_USERS);
  }
  uint8_t winner_id() const {
    return GetField<uint8_t>(VT_WINNER_ID, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_STATUS_CODE, 1) &&
           VerifyOffset(verifier, VT_USERS) &&
           verifier.VerifyVector(users()) &&
           VerifyField<uint8_t>(verifier, VT_WINNER_ID, 1) &&
           verifier.EndTable();
  }
};

struct GameStatusPushBuilder {
  typedef GameStatusPush Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_status_code(game::GameStatusCode status_code) {
    fbb_.AddElement<uint8_t>(GameStatusPush::VT_STATUS_CODE, static_cast<uint8_t>(status_code), 0);
  }
  void add_users(flatbuffers::Offset<flatbuffers::Vector<const game::UserStatus *>> users) {
    fbb_.AddOffset(GameStatusPush::VT_USERS, users);
  }
  void add_winner_id(uint8_t winner_id) {
    fbb_.AddElement<uint8_t>(GameStatusPush::VT_WINNER_ID, winner_id, 0);
  }
  explicit GameStatusPushBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<GameStatusPush> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<GameStatusPush>(end);
    return o;
  }
};

inline flatbuffers::Offset<GameStatusPush> CreateGameStatusPush(
    flatbuffers::FlatBufferBuilder &_fbb,
    game::GameStatusCode status_code = game::GameStatusCode_Wait,
    flatbuffers::Offset<flatbuffers::Vector<const game::UserStatus *>> users = 0,
    uint8_t winner_id = 0) {
  GameStatusPushBuilder builder_(_fbb);
  builder_.add_users(users);
  builder_.add_winner_id(winner_id);
  builder_.add_status_code(status_code);
  return builder_.Finish();
}

inline flatbuffers::Offset<GameStatusPush> CreateGameStatusPushDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    game::GameStatusCode status_code = game::GameStatusCode_Wait,
    const std::vector<game::UserStatus> *users = nullptr,
    uint8_t winner_id = 0) {
  auto users__ = users ? _fbb.CreateVectorOfStructs<game::UserStatus>(*users) : 0;
  return game::CreateGameStatusPush(
      _fbb,
      status_code,
      users__,
      winner_id);
}

struct SpawnEntityPush FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef SpawnEntityPushBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ENTITY_TYPE = 4,
    VT_ENTITY_STATUS = 6
  };
  game::EntityType entity_type() const {
    return static_cast<game::EntityType>(GetField<uint8_t>(VT_ENTITY_TYPE, 1));
  }
  const game::EntityStatus *entity_status() const {
    return GetStruct<const game::EntityStatus *>(VT_ENTITY_STATUS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_ENTITY_TYPE, 1) &&
           VerifyField<game::EntityStatus>(verifier, VT_ENTITY_STATUS, 4) &&
           verifier.EndTable();
  }
};

struct SpawnEntityPushBuilder {
  typedef SpawnEntityPush Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_entity_type(game::EntityType entity_type) {
    fbb_.AddElement<uint8_t>(SpawnEntityPush::VT_ENTITY_TYPE, static_cast<uint8_t>(entity_type), 1);
  }
  void add_entity_status(const game::EntityStatus *entity_status) {
    fbb_.AddStruct(SpawnEntityPush::VT_ENTITY_STATUS, entity_status);
  }
  explicit SpawnEntityPushBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<SpawnEntityPush> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<SpawnEntityPush>(end);
    return o;
  }
};

inline flatbuffers::Offset<SpawnEntityPush> CreateSpawnEntityPush(
    flatbuffers::FlatBufferBuilder &_fbb,
    game::EntityType entity_type = game::EntityType_Map,
    const game::EntityStatus *entity_status = nullptr) {
  SpawnEntityPushBuilder builder_(_fbb);
  builder_.add_entity_status(entity_status);
  builder_.add_entity_type(entity_type);
  return builder_.Finish();
}

struct ChangePlayerStatusReq FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ChangePlayerStatusReqBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ACTION = 4
  };
  const flatbuffers::Vector<uint8_t> *action() const {
    return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_ACTION);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_ACTION) &&
           verifier.VerifyVector(action()) &&
           verifier.EndTable();
  }
};

struct ChangePlayerStatusReqBuilder {
  typedef ChangePlayerStatusReq Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_action(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> action) {
    fbb_.AddOffset(ChangePlayerStatusReq::VT_ACTION, action);
  }
  explicit ChangePlayerStatusReqBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<ChangePlayerStatusReq> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ChangePlayerStatusReq>(end);
    return o;
  }
};

inline flatbuffers::Offset<ChangePlayerStatusReq> CreateChangePlayerStatusReq(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> action = 0) {
  ChangePlayerStatusReqBuilder builder_(_fbb);
  builder_.add_action(action);
  return builder_.Finish();
}

inline flatbuffers::Offset<ChangePlayerStatusReq> CreateChangePlayerStatusReqDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<uint8_t> *action = nullptr) {
  auto action__ = action ? _fbb.CreateVector<uint8_t>(*action) : 0;
  return game::CreateChangePlayerStatusReq(
      _fbb,
      action__);
}

struct CollisionEventPush FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef CollisionEventPushBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_COLLISION_CODE = 4,
    VT_ENTITY_A_STATUS = 6,
    VT_ENTITY_B_STATUS = 8
  };
  int32_t collision_code() const {
    return GetField<int32_t>(VT_COLLISION_CODE, 0);
  }
  const game::EntityStatus *entity_a_status() const {
    return GetStruct<const game::EntityStatus *>(VT_ENTITY_A_STATUS);
  }
  const game::EntityStatus *entity_b_status() const {
    return GetStruct<const game::EntityStatus *>(VT_ENTITY_B_STATUS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_COLLISION_CODE, 4) &&
           VerifyField<game::EntityStatus>(verifier, VT_ENTITY_A_STATUS, 4) &&
           VerifyField<game::EntityStatus>(verifier, VT_ENTITY_B_STATUS, 4) &&
           verifier.EndTable();
  }
};

struct CollisionEventPushBuilder {
  typedef CollisionEventPush Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_collision_code(int32_t collision_code) {
    fbb_.AddElement<int32_t>(CollisionEventPush::VT_COLLISION_CODE, collision_code, 0);
  }
  void add_entity_a_status(const game::EntityStatus *entity_a_status) {
    fbb_.AddStruct(CollisionEventPush::VT_ENTITY_A_STATUS, entity_a_status);
  }
  void add_entity_b_status(const game::EntityStatus *entity_b_status) {
    fbb_.AddStruct(CollisionEventPush::VT_ENTITY_B_STATUS, entity_b_status);
  }
  explicit CollisionEventPushBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<CollisionEventPush> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<CollisionEventPush>(end);
    return o;
  }
};

inline flatbuffers::Offset<CollisionEventPush> CreateCollisionEventPush(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t collision_code = 0,
    const game::EntityStatus *entity_a_status = nullptr,
    const game::EntityStatus *entity_b_status = nullptr) {
  CollisionEventPushBuilder builder_(_fbb);
  builder_.add_entity_b_status(entity_b_status);
  builder_.add_entity_a_status(entity_a_status);
  builder_.add_collision_code(collision_code);
  return builder_.Finish();
}

struct ChangeEntityStatusPush FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ChangeEntityStatusPushBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ACTION = 4,
    VT_STATUS = 6
  };
  const flatbuffers::Vector<uint8_t> *action() const {
    return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_ACTION);
  }
  const game::EntityStatus *status() const {
    return GetStruct<const game::EntityStatus *>(VT_STATUS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_ACTION) &&
           verifier.VerifyVector(action()) &&
           VerifyField<game::EntityStatus>(verifier, VT_STATUS, 4) &&
           verifier.EndTable();
  }
};

struct ChangeEntityStatusPushBuilder {
  typedef ChangeEntityStatusPush Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_action(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> action) {
    fbb_.AddOffset(ChangeEntityStatusPush::VT_ACTION, action);
  }
  void add_status(const game::EntityStatus *status) {
    fbb_.AddStruct(ChangeEntityStatusPush::VT_STATUS, status);
  }
  explicit ChangeEntityStatusPushBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<ChangeEntityStatusPush> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ChangeEntityStatusPush>(end);
    return o;
  }
};

inline flatbuffers::Offset<ChangeEntityStatusPush> CreateChangeEntityStatusPush(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> action = 0,
    const game::EntityStatus *status = nullptr) {
  ChangeEntityStatusPushBuilder builder_(_fbb);
  builder_.add_status(status);
  builder_.add_action(action);
  return builder_.Finish();
}

inline flatbuffers::Offset<ChangeEntityStatusPush> CreateChangeEntityStatusPushDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<uint8_t> *action = nullptr,
    const game::EntityStatus *status = nullptr) {
  auto action__ = action ? _fbb.CreateVector<uint8_t>(*action) : 0;
  return game::CreateChangeEntityStatusPush(
      _fbb,
      action__,
      status);
}

struct ChangePlayerStatusPush FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ChangePlayerStatusPushBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_JUMPABLE = 4,
    VT_SHOOTABLE = 6
  };
  bool jumpable() const {
    return GetField<uint8_t>(VT_JUMPABLE, 0) != 0;
  }
  bool shootable() const {
    return GetField<uint8_t>(VT_SHOOTABLE, 0) != 0;
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_JUMPABLE, 1) &&
           VerifyField<uint8_t>(verifier, VT_SHOOTABLE, 1) &&
           verifier.EndTable();
  }
};

struct ChangePlayerStatusPushBuilder {
  typedef ChangePlayerStatusPush Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_jumpable(bool jumpable) {
    fbb_.AddElement<uint8_t>(ChangePlayerStatusPush::VT_JUMPABLE, static_cast<uint8_t>(jumpable), 0);
  }
  void add_shootable(bool shootable) {
    fbb_.AddElement<uint8_t>(ChangePlayerStatusPush::VT_SHOOTABLE, static_cast<uint8_t>(shootable), 0);
  }
  explicit ChangePlayerStatusPushBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<ChangePlayerStatusPush> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ChangePlayerStatusPush>(end);
    return o;
  }
};

inline flatbuffers::Offset<ChangePlayerStatusPush> CreateChangePlayerStatusPush(
    flatbuffers::FlatBufferBuilder &_fbb,
    bool jumpable = false,
    bool shootable = false) {
  ChangePlayerStatusPushBuilder builder_(_fbb);
  builder_.add_shootable(shootable);
  builder_.add_jumpable(jumpable);
  return builder_.Finish();
}

struct PingPush FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PingPushBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_DELAY = 4
  };
  int32_t delay() const {
    return GetField<int32_t>(VT_DELAY, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_DELAY, 4) &&
           verifier.EndTable();
  }
};

struct PingPushBuilder {
  typedef PingPush Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_delay(int32_t delay) {
    fbb_.AddElement<int32_t>(PingPush::VT_DELAY, delay, 0);
  }
  explicit PingPushBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<PingPush> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<PingPush>(end);
    return o;
  }
};

inline flatbuffers::Offset<PingPush> CreatePingPush(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t delay = 0) {
  PingPushBuilder builder_(_fbb);
  builder_.add_delay(delay);
  return builder_.Finish();
}

struct PingAck FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PingAckBuilder Builder;
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct PingAckBuilder {
  typedef PingAck Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  explicit PingAckBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<PingAck> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<PingAck>(end);
    return o;
  }
};

inline flatbuffers::Offset<PingAck> CreatePingAck(
    flatbuffers::FlatBufferBuilder &_fbb) {
  PingAckBuilder builder_(_fbb);
  return builder_.Finish();
}

}  // namespace game

#endif  // FLATBUFFERS_GENERATED_GAMEPACKET_GAME_H_