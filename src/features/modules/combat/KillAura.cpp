#include "KillAura.h"

#include "src/sdk/game/world/actor/Actor.h"
#include "src/utils/ActorUtils.h"

#include "Main.h"
#include "src/utils/Logger.h"
#include "src/sdk/game/world/actor/mob/player/gamemode/GameMode.h"
#include "src/sdk/game/world/actor/mob/player/LocalPlayer.h"

#include "../../FeatureManager.h"
#include "src/sdk/game/world/level/Level.h"
#include "src/sdk/game/world/level/dimension/Dimension.h"

#include "src/hook/ActorHook.h"
#include "src/sdk/game/world/actor/components/ActorTypeComponent.hpp"

void KillAura::onEnable() {
	gFeatureManager->mDispatcher->listen<NormalTickEvent, &KillAura::onTickEvent>(this);
}

void KillAura::onDisable() {
	gFeatureManager->mDispatcher->deafen<NormalTickEvent, &KillAura::onTickEvent>(this);
}


void KillAura::onTickEvent(NormalTickEvent& event) {
	LocalPlayer* player = event.mActor;
	if (player->getLevel() == nullptr) {
		return;
	}
	auto actors = player->getLevel()->getPlayers();
	static int64_t lastattack = 0;
	int64_t now = NOW;
	float aps = mAPS.getRandom();
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
		player->getGameMode()->attack(actor);
	    player->swing();
		lastattack = NOW;
		break;
	}
}
