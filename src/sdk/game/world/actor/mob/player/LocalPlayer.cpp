#include "LocalPlayer.h"

#include "Main.h"

#include "src/sdk/SigManager.h"
#include "src/utils/Logger.h"


void LocalPlayer::swing() {
	auto result = SigManager::get(SignatureID::LocalPlayer_swing);
	if (!result) {
		Logger::error("Failed to find LocalPlayer::swing");
	}
	auto call = (void(__fastcall*)(void*)) (*result);
	return call(this);
}