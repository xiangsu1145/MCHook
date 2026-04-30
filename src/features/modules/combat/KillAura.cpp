#include "KillAura.h"

#include "src/sdk/game/world/actor/Actor.h"
#include "src/utils/ActorUtils.h"

#include "Main.h"
#include "src/utils/Logger.h"
#include "src/sdk/game/world/actor/mob/player/gamemode/GameMode.h"
#include "src/sdk/game/world/actor/mob/player/LocalPlayer.h"
#include "src/sdk/game/ClientInstance.h"
#include "../../FeatureManager.h"
#include "src/sdk/game/world/level/Level.h"
#include "src/sdk/game/world/level/dimension/Dimension.h"

#include "src/hook/ActorHook.h"
#include "src/sdk/game/world/actor/components/ActorTypeComponent.hpp"
#include "src/sdk/game/network/packet/PlayerAuthInputPacket.h"

void KillAura::onEnable() {
	gFeatureManager->mDispatcher->listen<NormalTickEvent, &KillAura::onTickEvent>(this);
	gFeatureManager->mDispatcher->listen<PacketOutEvent, &KillAura::onPacketOutEvent>(this);
	gFeatureManager->mDispatcher->listen<RenderEvent, &KillAura::onRenderEvent>(this);
}

void KillAura::onDisable() {
	gFeatureManager->mDispatcher->deafen<NormalTickEvent, &KillAura::onTickEvent>(this);
	gFeatureManager->mDispatcher->deafen<PacketOutEvent, &KillAura::onPacketOutEvent>(this);
	gFeatureManager->mDispatcher->deafen<RenderEvent, &KillAura::onRenderEvent>(this);
}

void KillAura::rotate(Actor* actor) {
	mRotating = true;
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

	bool foundAttackable = false;

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

		foundAttackable = true;

		player->getGameMode()->attack(actor);
	    player->swing();
		Vec3 myPos = *player->getPosition();
		Vec3 targetPos = *actor->getPosition();
		mRot = myPos.CalcAngle(targetPos);
		rotate(actor);
		lastattack = NOW;
		break;
	}

	if (!foundAttackable) {
		mRotating = false;
	}
}

void KillAura::onPacketOutEvent(PacketOutEvent& event) {
	if (event.mPacket->getId() == MinecraftPacketIds::PlayerAuthInputPacket) {
		auto auth = event.getPacket<PlayerAuthInputPacket>();
		if (mRotating) {
			// 1. 保存原始视角（来自玩家原始输入/客户端真实视角）
			Vec2 origRot = auth->mRot;

			// 2. 计算水平旋转差（只关心 yaw）
			float deltaYaw = mRot.y - origRot.y;

			// 3. 旋转移动向量，补偿视角变化
			float rad = deltaYaw * (3.1415926535f / 180.0f); // 角度转弧度
			float s = sinf(rad);
			float c = cosf(rad);

			Vec2 move = auth->mAnalogMoveVector; // x=侧移, y=前后
			float newX = move.x * c + move.y * s;
			float newY = -move.x * s + move.y * c;

			auth->mAnalogMoveVector = Vec2(newX, newY);

			// 4. 设置强制视角
			auth->mRot = mRot;
			auth->mYHeadRot = mRot.y;
		}
	}
}

void KillAura::onRenderEvent(RenderEvent& event) {
	auto player = ClientInstance::get()->getLocalPlayer();
	if (!player) {
		return;
	}
	if (mRotating) {
		auto rot = player->getActorRot();
		//rot->mRotationDegree = mRot;
		//rot->mRotationDegreePrevious = mRot;
	}
}