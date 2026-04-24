#include "GameMode.h"

void GameMode::attack(Actor* entity) {
	return reinterpret_cast<void (__fastcall*)(GameMode*, Actor*)>(vtable[14])(this, entity);
}