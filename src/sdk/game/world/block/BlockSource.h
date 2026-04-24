#pragma once

#include "Block.h"

#include "src/utils/Utils.h"

class BlockSource
{
public:
	virtual ~BlockSource();
	virtual Block* getBlock(int x, int y, int z) = 0;
	virtual Block* getBlock(BlockPos& pos) = 0;
};

