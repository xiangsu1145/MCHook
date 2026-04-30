#pragma once
#include "ActorType.hpp"
#include "ActorUniqueID.hpp"
#include "src/utils/MemoryUtils.h"
#include "src/utils/Utils.h"
#include "components/StateVectorComponent.h"
#include "components/ActorRotationComponent.h"
#include "../block/BlockSource.h"
class LocalPlayer;
class Level;
class Dimension;

class Actor{
public:
	float distanceTo(Actor* actor);
	Vec3* getPosition();
	StateVectorComponent* getStateVector();
	ActorRotationComponent* getActorRot();
	Vec3* getPositionPrev();
	Vec3* getPosDelta();
	void setPosition(Vec3 &pos);
	Level*getLevel();
	Dimension *getDimension();
	BlockSource* getRegion();
	float getFallDistance();
};

