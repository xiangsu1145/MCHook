#pragma once
#include <vector>
#include "src/sdk/game/world/actor/Actor.h"

class ActorUtils{
public:
	static std::vector<Actor*> getActorList(ActorType filterType);
};

