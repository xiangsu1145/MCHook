#include "Player.h"

#include "Main.h"
#include "src/sdk/SigManager.h"
#include "src/utils/Logger.h"

GameMode* Player::getGameMode() {
	auto result = SigManager::get(SignatureID::Player_getGameMode);
	if (!result) {
		Logger::error("Failed to find Player::getGameMode");
	}
	auto call = (GameMode * (__fastcall*)(void*)) (*result);
	return call(this);
}