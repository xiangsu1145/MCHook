#pragma once

#include "BlockLegacy.h"

#include "src/utils/MemoryUtils.h"

class Block
{
public:
	BlockLegacy* toLegacy() {
		return reinterpret_cast<BlockLegacy*>(reinterpret_cast<uintptr_t>(this) + 0xE0);

	};

};

