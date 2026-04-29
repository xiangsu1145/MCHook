#pragma once

#include "src/utils/Utils.h"

class StateVectorComponent
{
public:
	Vec3 mPos;
	Vec3 mPosPrev;
	Vec3 mPosDelta;
};