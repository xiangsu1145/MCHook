#include "Fly.h"
#include "../../FeatureManager.h"
#include "src/sdk/game/network/packet/PlayerAuthInputPacket.h"
#include "src/sdk/game/ClientInstance.h"
#include "src/sdk/game/world/actor/mob/player/LocalPlayer.h"

void Fly::onEnable() {
	gFeatureManager->mDispatcher->listen<PacketOutEvent, &Fly::onPacketOutEvent>(this);
}

void Fly::onDisable() {
	gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Fly::onPacketOutEvent>(this);
}

void Fly::onPacketOutEvent(PacketOutEvent& event) {
	if (event.mPacket->getId() != MinecraftPacketIds::PlayerAuthInputPacket) return;

	auto auth = event.getPacket<PlayerAuthInputPacket>();
	auto player = ClientInstance::get()->getLocalPlayer();
	if (!player) return;

	Vec2 rot = auth->mRot;
	Vec2 move = auth->mAnalogMoveVector;

	float yaw = (rot.y + 90.0f) * (3.1415926535f / 180.0f);
	float sinYaw = std::sin(yaw);
	float cosYaw = std::cos(yaw);

	float side = move.x;
	float forward = move.y;

	float worldX = (side * sinYaw) + (forward * cosYaw);
	float worldZ = (-side * cosYaw) + (forward * sinYaw);

	if (mMode.mValue == Mode::Motion) {
		auto posDelta = player->getPosDelta();
		posDelta->x = worldX * mSpeed.mValue;
		posDelta->z = worldZ * mSpeed.mValue;
		posDelta->y = 0.0f;

		if (mBypass.mValue) {
			posDelta->y = std::sin(GetTickCount() * 0.1f) * 0.02f;
		}
	} else if (mMode.mValue == Mode::Position) {
		auto pos = player->getPosition();
		Vec3 newPos = *pos;
		newPos.x += worldX * mSpeed.mValue;
		newPos.z += worldZ * mSpeed.mValue;
		player->setPosition(newPos);
		auto posDelta = player->getPosDelta();
		posDelta->y = 0.0f;
		if (mBypass.mValue) {
			newPos.y += std::sin(GetTickCount() * 0.1f) * 0.02f;
		}
		player->setPosition(newPos);
	}
}