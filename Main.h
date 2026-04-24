#pragma once

#include "windows.h"

#include "../src/Utils/stdint.h"

#include <chrono>

#define NOW std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

class Main{
public:
	static int64 baseAddress;
	static HMODULE hModule;
	static void init(const HMODULE hModule);
};

