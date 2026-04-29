#pragma once

#include <memory>
#include "MinecraftPacketIds.hpp"
#include "packet/Packet.h"


class MinecraftPackets
{
public:
	static std::shared_ptr<Packet> createPacket(MinecraftPacketIds packetId);
};

