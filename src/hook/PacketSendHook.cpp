#include "PacketSendHook.h"

#include "src/sdk/SigManager.h"
#include "src/utils/Logger.h"
#include <MinHook.h>

#include "src/sdk/game/network/packet/Packet.h"
#include "src/features/event/PacketOutEvent.h"
#include "src/features/FeatureManager.h"

void PacketSendHook::hookSendToServer(LoopbackPacketSender* thiz, Packet* packet) {
	auto holder = nes::make_holder<PacketOutEvent>(packet);
	gFeatureManager->mDispatcher->trigger(holder);
	if (holder->isCanceled()) {
		return;
	}
	return originalFunc(thiz, packet);
}

void PacketSendHook::init() {
	auto result = SigManager::get(SignatureID::LoopbackPacketSender_sendToServer);
	if (!result) {
		Logger::error("Failed to find LoopbackPacketSender::sendToServer");
		return;
	}
	void* target = (void*)(*result);
	MH_CreateHook(target, &hookSendToServer, (void**)&originalFunc);
	MH_EnableHook(target);
}