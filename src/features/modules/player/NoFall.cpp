#include "NoFall.h"
#include "../../FeatureManager.h"
#include "src/sdk/game/network/packet/PlayerAuthInputPacket.h"
#include "src/utils/Logger.h"
#include "src/sdk/game/ClientInstance.h"
#include "src/sdk/game/world/actor/mob/player/LocalPlayer.h"

void NoFall::onEnable() {
	gFeatureManager->mDispatcher->listen<PacketOutEvent, &NoFall::onPacketOutEvent>(this);
}

void NoFall::onDisable() {
	gFeatureManager->mDispatcher->deafen<PacketOutEvent, &NoFall::onPacketOutEvent>(this);
}

void NoFall::onPacketOutEvent(PacketOutEvent& event) {
	if (event.mPacket->getId() == MinecraftPacketIds::PlayerAuthInputPacket) {
		auto auth = event.getPacket<PlayerAuthInputPacket>();
		auto player = ClientInstance::get()->getLocalPlayer();
		
		if (player->getFallDistance() < mFallDistanceMin.mValue || player->getFallDistance() > mFallDistanceMax.mValue) {
			return;
		}
		
		if (mMode.mValue == Mode::Gliding) {
			auth->mInputFlags |= (InputData::StartGliding);
			auth->mInputFlags |= InputData::Jumping;
			auth->mInputFlags |= InputData::StartJumping;
			auth->mPosDelta.y = -0.000001;
		}
		if (mMode.mValue == Mode::NoAuthPacket) {
			if (!auth->mOnGround) {
				event.cancel();
			}
		}
		if (mMode.mValue == Mode::Blink) {
			static Vec3 lastPos = *player->getPosition();
			if (auth->mOnGround) {
				lastPos = auth->mPos;
			}
			else {
				auth->mPos = lastPos;
			}
		}
	}
}