#include "LoopbackPacketSender.h"
#include "src/sdk/SigManager.h"
#include "src/utils/Logger.h"

void LoopbackPacketSender::sendToServer(Packet* packet) {
	auto result = SigManager::get(SignatureID::LoopbackPacketSender_sendToServer);
	if (!result) {
		Logger::error("Failed to find LoopbackPacketSender::sendToServer");
		return;
	}
	auto call = (void(__fastcall*)(LoopbackPacketSender*, Packet*)) (*result);
	return call(this, packet);
}