#pragma once
#include "../Mob.h"

class GameMode;

class Player : public Mob{
public:
	GameMode* getGameMode();
};

