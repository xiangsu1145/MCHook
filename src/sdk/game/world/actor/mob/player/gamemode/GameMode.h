#pragma once

#include "src/utils/stdint.h"

class Actor;

class GameMode{
public:
	uintptr** vtable;
	void attack(Actor* entity);
};

