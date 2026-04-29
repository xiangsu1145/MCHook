#include "MinecraftPackets.h"
#include "src/sdk/SigManager.h"
#include "src/utils/Logger.h"

std::shared_ptr<Packet> MinecraftPackets::createPacket(MinecraftPacketIds packetId) {
	auto result = SigManager::get(SignatureID::MinecraftPackets_createPacket);
	if (!result) {
		Logger::error("Failed to find MinecraftPackets::createPacket");
	}
	auto call = (std::shared_ptr<Packet>(__fastcall*)(MinecraftPacketIds)) (*result);
	return call(packetId);
}