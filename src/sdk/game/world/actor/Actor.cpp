#include "Actor.h"

#include "Main.h"

#include "src/sdk/SigManager.h"
#include "src/utils/Logger.h"

Vec3* Actor::getPosition() {
	auto result = SigManager::get(SignatureID::Actor_getPosition);
	if (!result) {
		Logger::error("Failed to find Actor::getPostion");
	}
	auto call = (Vec3 * (__fastcall*)(void*)) (*result);
	return call(this);
}

float Actor::distanceTo(Actor* actor) {
	auto pos1 = this->getPosition();
	auto pos2 = actor->getPosition();
	if (!pos1 || !pos2) return -1.f;
	return pos1->distance(*pos2);
}

Level *Actor::getLevel() {
	auto result = SigManager::get(SignatureID::Actor_getLevel);
	if (!result) {
		Logger::error("Failed to find Actor::getLevel");
	}
	auto call = (Level * (__fastcall*)(void*)) (*result);
	return call(this);
}

Dimension* Actor::getDimension() {
	auto result = SigManager::get(SignatureID::Actor_getDimension);
	if (!result) {
		Logger::error("Failed to find Actor::getDimension");
	}
	auto call = (Dimension * (__fastcall*)(void*)) (*result);
	return call(this);
}

BlockSource* Actor::getRegion() {
	auto result = SigManager::get(SignatureID::Actor_getRegion);
	if (!result) {
		Logger::error("Failed to find Actor::getRegion");
	}
	auto call = (BlockSource * (__fastcall*)(void*)) (*result);
	return call(this);
}

float Actor::getFallDistance() {
	auto result = SigManager::get(SignatureID::Actor_getFallDistance);
	if (!result) {
		Logger::error("Failed to find Actor::getFallDistance");
	}
	auto call = (float (__fastcall*)(void*)) (*result);
	return call(this);
}

