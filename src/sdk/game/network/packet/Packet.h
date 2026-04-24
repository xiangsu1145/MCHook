#pragma once

#include <string>
#include <cstdint>

#include "../MinecraftPacketIds.hpp"
#include "IPacketHandlerDispatcher.h"

enum class PacketPriority {
    High,
    Immediate,
    Low,
    Medium,
    Count
};

namespace NetworkPeer {
    enum class Reliability {
        Reliable,
        ReliableOrdered,
        Unreliable,
        UnreliableSequenced
    };
}

enum class Compressibility {
    Compressible,
    Incompressible
};

class Packet {
public:
    PacketPriority mPriority;
    NetworkPeer::Reliability mReliability;
    uint64_t mSenderSubId;
    char pad001[0x8];
    IPacketHandlerDispatcher const* mHandler;
    Compressibility mCompressible;
    char pad002[0x4];
public:
    // vIndex: 0
    virtual ~Packet() = default;

    // vIndex: 1
    virtual MinecraftPacketIds getId() = 0;

    // vIndex: 2
    virtual std::string getName() = 0;

    // vIndex: 3
    virtual void*
        checkSize(uint64_t packetSize, bool receiverIsServer) = 0;

    // vIndex: 4
    virtual void write(class BinaryStream&) = 0;

    // vIndex: 5
    virtual void*
        read(class ReadOnlyBinaryStream& bitStream) = 0;

    // vIndex: 6
    virtual bool disallowBatching() = 0;

    // vIndex: 7
    virtual bool isValid() = 0;

    // vIndex: 8
    virtual void*
        _read(class ReadOnlyBinaryStream&) = 0;
};


static_assert(sizeof(Packet) == 0x30, "Packet size is incorrect");