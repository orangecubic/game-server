// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_PROTOCOL_GAME_H_
#define FLATBUFFERS_GENERATED_PROTOCOL_GAME_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 22 &&
              FLATBUFFERS_VERSION_MINOR == 12 &&
              FLATBUFFERS_VERSION_REVISION == 6,
             "Non-compatible flatbuffers version included");

#include "game_packet_generated.h"

namespace game {

struct Packet;
struct PacketBuilder;

enum Payload : uint8_t {
  Payload_NONE = 0,
  Payload_ConnectReq = 1,
  Payload_ConnectRep = 2,
  Payload_MatchReq = 3,
  Payload_MatchRep = 4,
  Payload_BattleReadyReq = 5,
  Payload_GameStatusPush = 6,
  Payload_ChangePlayerStatusReq = 7,
  Payload_SpawnEntityPush = 8,
  Payload_ChangeEntityStatusPush = 9,
  Payload_CollisionEventPush = 10,
  Payload_PingPush = 11,
  Payload_PingAck = 12,
  Payload_ChangePlayerStatusPush = 13,
  Payload_MIN = Payload_NONE,
  Payload_MAX = Payload_ChangePlayerStatusPush
};

inline const Payload (&EnumValuesPayload())[14] {
  static const Payload values[] = {
    Payload_NONE,
    Payload_ConnectReq,
    Payload_ConnectRep,
    Payload_MatchReq,
    Payload_MatchRep,
    Payload_BattleReadyReq,
    Payload_GameStatusPush,
    Payload_ChangePlayerStatusReq,
    Payload_SpawnEntityPush,
    Payload_ChangeEntityStatusPush,
    Payload_CollisionEventPush,
    Payload_PingPush,
    Payload_PingAck,
    Payload_ChangePlayerStatusPush
  };
  return values;
}

inline const char * const *EnumNamesPayload() {
  static const char * const names[15] = {
    "NONE",
    "ConnectReq",
    "ConnectRep",
    "MatchReq",
    "MatchRep",
    "BattleReadyReq",
    "GameStatusPush",
    "ChangePlayerStatusReq",
    "SpawnEntityPush",
    "ChangeEntityStatusPush",
    "CollisionEventPush",
    "PingPush",
    "PingAck",
    "ChangePlayerStatusPush",
    nullptr
  };
  return names;
}

inline const char *EnumNamePayload(Payload e) {
  if (flatbuffers::IsOutRange(e, Payload_NONE, Payload_ChangePlayerStatusPush)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesPayload()[index];
}

template<typename T> struct PayloadTraits {
  static const Payload enum_value = Payload_NONE;
};

template<> struct PayloadTraits<game::ConnectReq> {
  static const Payload enum_value = Payload_ConnectReq;
};

template<> struct PayloadTraits<game::ConnectRep> {
  static const Payload enum_value = Payload_ConnectRep;
};

template<> struct PayloadTraits<game::MatchReq> {
  static const Payload enum_value = Payload_MatchReq;
};

template<> struct PayloadTraits<game::MatchRep> {
  static const Payload enum_value = Payload_MatchRep;
};

template<> struct PayloadTraits<game::BattleReadyReq> {
  static const Payload enum_value = Payload_BattleReadyReq;
};

template<> struct PayloadTraits<game::GameStatusPush> {
  static const Payload enum_value = Payload_GameStatusPush;
};

template<> struct PayloadTraits<game::ChangePlayerStatusReq> {
  static const Payload enum_value = Payload_ChangePlayerStatusReq;
};

template<> struct PayloadTraits<game::SpawnEntityPush> {
  static const Payload enum_value = Payload_SpawnEntityPush;
};

template<> struct PayloadTraits<game::ChangeEntityStatusPush> {
  static const Payload enum_value = Payload_ChangeEntityStatusPush;
};

template<> struct PayloadTraits<game::CollisionEventPush> {
  static const Payload enum_value = Payload_CollisionEventPush;
};

template<> struct PayloadTraits<game::PingPush> {
  static const Payload enum_value = Payload_PingPush;
};

template<> struct PayloadTraits<game::PingAck> {
  static const Payload enum_value = Payload_PingAck;
};

template<> struct PayloadTraits<game::ChangePlayerStatusPush> {
  static const Payload enum_value = Payload_ChangePlayerStatusPush;
};

bool VerifyPayload(flatbuffers::Verifier &verifier, const void *obj, Payload type);
bool VerifyPayloadVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types);

struct Packet FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PacketBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_VERSION = 4,
    VT_MESSAGES_TYPE = 6,
    VT_MESSAGES = 8
  };
  uint16_t version() const {
    return GetField<uint16_t>(VT_VERSION, 0);
  }
  const flatbuffers::Vector<uint8_t> *messages_type() const {
    return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_MESSAGES_TYPE);
  }
  const flatbuffers::Vector<flatbuffers::Offset<void>> *messages() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<void>> *>(VT_MESSAGES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint16_t>(verifier, VT_VERSION, 2) &&
           VerifyOffset(verifier, VT_MESSAGES_TYPE) &&
           verifier.VerifyVector(messages_type()) &&
           VerifyOffset(verifier, VT_MESSAGES) &&
           verifier.VerifyVector(messages()) &&
           VerifyPayloadVector(verifier, messages(), messages_type()) &&
           verifier.EndTable();
  }
};

struct PacketBuilder {
  typedef Packet Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_version(uint16_t version) {
    fbb_.AddElement<uint16_t>(Packet::VT_VERSION, version, 0);
  }
  void add_messages_type(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> messages_type) {
    fbb_.AddOffset(Packet::VT_MESSAGES_TYPE, messages_type);
  }
  void add_messages(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<void>>> messages) {
    fbb_.AddOffset(Packet::VT_MESSAGES, messages);
  }
  explicit PacketBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Packet> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Packet>(end);
    return o;
  }
};

inline flatbuffers::Offset<Packet> CreatePacket(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint16_t version = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> messages_type = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<void>>> messages = 0) {
  PacketBuilder builder_(_fbb);
  builder_.add_messages(messages);
  builder_.add_messages_type(messages_type);
  builder_.add_version(version);
  return builder_.Finish();
}

inline flatbuffers::Offset<Packet> CreatePacketDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint16_t version = 0,
    const std::vector<uint8_t> *messages_type = nullptr,
    const std::vector<flatbuffers::Offset<void>> *messages = nullptr) {
  auto messages_type__ = messages_type ? _fbb.CreateVector<uint8_t>(*messages_type) : 0;
  auto messages__ = messages ? _fbb.CreateVector<flatbuffers::Offset<void>>(*messages) : 0;
  return game::CreatePacket(
      _fbb,
      version,
      messages_type__,
      messages__);
}

inline bool VerifyPayload(flatbuffers::Verifier &verifier, const void *obj, Payload type) {
  switch (type) {
    case Payload_NONE: {
      return true;
    }
    case Payload_ConnectReq: {
      auto ptr = reinterpret_cast<const game::ConnectReq *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_ConnectRep: {
      auto ptr = reinterpret_cast<const game::ConnectRep *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_MatchReq: {
      auto ptr = reinterpret_cast<const game::MatchReq *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_MatchRep: {
      auto ptr = reinterpret_cast<const game::MatchRep *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_BattleReadyReq: {
      auto ptr = reinterpret_cast<const game::BattleReadyReq *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_GameStatusPush: {
      auto ptr = reinterpret_cast<const game::GameStatusPush *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_ChangePlayerStatusReq: {
      auto ptr = reinterpret_cast<const game::ChangePlayerStatusReq *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_SpawnEntityPush: {
      auto ptr = reinterpret_cast<const game::SpawnEntityPush *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_ChangeEntityStatusPush: {
      auto ptr = reinterpret_cast<const game::ChangeEntityStatusPush *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_CollisionEventPush: {
      auto ptr = reinterpret_cast<const game::CollisionEventPush *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_PingPush: {
      auto ptr = reinterpret_cast<const game::PingPush *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_PingAck: {
      auto ptr = reinterpret_cast<const game::PingAck *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Payload_ChangePlayerStatusPush: {
      auto ptr = reinterpret_cast<const game::ChangePlayerStatusPush *>(obj);
      return verifier.VerifyTable(ptr);
    }
    default: return true;
  }
}

inline bool VerifyPayloadVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types) {
  if (!values || !types) return !values && !types;
  if (values->size() != types->size()) return false;
  for (flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
    if (!VerifyPayload(
        verifier,  values->Get(i), types->GetEnum<Payload>(i))) {
      return false;
    }
  }
  return true;
}

inline const game::Packet *GetPacket(const void *buf) {
  return flatbuffers::GetRoot<game::Packet>(buf);
}

inline const game::Packet *GetSizePrefixedPacket(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<game::Packet>(buf);
}

inline bool VerifyPacketBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<game::Packet>(nullptr);
}

inline bool VerifySizePrefixedPacketBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<game::Packet>(nullptr);
}

inline void FinishPacketBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<game::Packet> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedPacketBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<game::Packet> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace game

#endif  // FLATBUFFERS_GENERATED_PROTOCOL_GAME_H_
