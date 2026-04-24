#pragma once
#include "ActorType.hpp"
#include "ActorUniqueID.hpp"
#include "src/utils/MemoryUtils.h"
#include "src/utils/Utils.h"

#include "EntityContext.hpp"
#include "../block/BlockSource.h"
class LocalPlayer;
class Level;
class Dimension;

class Actor{
public:

	float distanceTo(Actor* actor);
	Vec3 *getPosition();
	Level*getLevel();
	Dimension *getDimension();
	EntityContext& getEntityContext();
	BlockSource* getRegion();
	float getFallDistance();
};

