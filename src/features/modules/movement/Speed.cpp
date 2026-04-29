#include "Speed.h"
#include "../../FeatureManager.h"
#include "src/sdk/game/network/packet/PlayerAuthInputPacket.h"
#include "src/sdk/game/ClientInstance.h"
#include "src/sdk/game/world/actor/mob/player/LocalPlayer.h"

void Speed::onEnable() {
	gFeatureManager->mDispatcher->listen<PacketOutEvent, &Speed::onPacketOutEvent>(this);
}

void Speed::onDisable() {
	gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Speed::onPacketOutEvent>(this);
}

void Speed::onPacketOutEvent(PacketOutEvent& event) {
	if (event.mPacket->getId() == MinecraftPacketIds::PlayerAuthInputPacket) {
		auto auth = event.getPacket<PlayerAuthInputPacket>();
		auto player = ClientInstance::get()->getLocalPlayer();

		Vec2 rot = auth->mRot;
		Vec2 move = auth->mAnalogMoveVector;
		if (move.x == 0.0f && move.y == 0.0f) {
			return;
		}
		float yaw = (rot.y + 90.0f) * (3.1415926535f / 180.0f);
		float sinYaw = std::sin(yaw);
		float cosYaw = std::cos(yaw);

		float side = move.x;
		float forward = move.y;

		float worldX = (side * sinYaw) + (forward * cosYaw);
		float worldZ = (-side * cosYaw) + (forward * sinYaw);

		auto posDelta = player->getPosDelta();
		posDelta->x = worldX * mSpeed.mValue;
		posDelta->z = worldZ * mSpeed.mValue;

	}
}