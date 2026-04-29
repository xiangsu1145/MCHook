#include "InfiniteAura.h"
#include "../../FeatureManager.h"
#include "src/sdk/game/network/packet/PlayerAuthInputPacket.h"
#include "src/sdk/game/ClientInstance.h"
#include "src/sdk/game/world/actor/mob/player/LocalPlayer.h"
#include "src/sdk/game/world/actor/mob/player/gamemode/GameMode.h"
#include "src/sdk/game/world/level/Level.h"
#include "Main.h"
#include "src/sdk/game/network/MinecraftPackets.h"
#include "src/sdk/game/network/LoopbackPacketSender.h"

void InfiniteAura::onEnable() {
	gFeatureManager->mDispatcher->listen<NormalTickEvent, &InfiniteAura::onTickEvent>(this);
}

void InfiniteAura::onDisable() {
	gFeatureManager->mDispatcher->deafen<NormalTickEvent, &InfiniteAura::onTickEvent>(this);
}

void InfiniteAura::onTickEvent(NormalTickEvent& event) {
	auto player = event.mActor;
	if (player->getLevel() == nullptr) {
		return;
	}
	auto actors = player->getLevel()->getPlayers();
	static int64_t lastattack = 0;
	int64_t now = NOW;
	float aps = mAPS.get();
	int64_t delay = 1000 / aps;

	for (auto actor : actors) {
		if (actor == player) {
			continue;
		}
		if (actor->distanceTo(player) > mRange.mValue) {
			continue;
		}
		if (now - lastattack < delay) {
			break;
		}
		auto mPacket = MinecraftPackets::createPacket(MinecraftPacketIds::PlayerAuthInputPacket);
		auto auth = static_cast<PlayerAuthInputPacket*>(mPacket.get());
		auth->mPos = *actor->getPosition();
		Vec3 lastPos = *actor->getPosition();
		LoopbackPacketSender::getSender()->sendToServer(auth);
		player->setPosition(*actor->getPosition());
		player->getGameMode()->attack(actor);
		player->swing();
		player->setPosition(lastPos);
		auth->mPos = lastPos;
		LoopbackPacketSender::getSender()->sendToServer(auth);
		lastattack = NOW;
		break;
	}
}