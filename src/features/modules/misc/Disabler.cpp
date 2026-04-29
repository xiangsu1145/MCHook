#include "Disabler.h"
#include "../../FeatureManager.h"
#include "src/sdk/game/network/packet/PlayerAuthInputPacket.h"

void Disabler::onEnable() {
	gFeatureManager->mDispatcher->listen<PacketOutEvent, &Disabler::onPacketOutEvent>(this);
}

void Disabler::onDisable() {
	gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Disabler::onPacketOutEvent>(this);
}

void Disabler::onPacketOutEvent(PacketOutEvent& event) {
	if (event.mPacket->getId() == MinecraftPacketIds::PlayerAuthInputPacket) {
		auto auth = event.getPacket<PlayerAuthInputPacket>();
		if (mMode.mValue == Mode::Microsoft) {
			auth->mHasfalldamage = true;//easy only netease
		}
	}
}