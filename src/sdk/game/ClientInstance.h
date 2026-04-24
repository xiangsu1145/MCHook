#pragma once

#include <cstdint>
#include "world/Minecraft.h"

class LocalPlayer;

class ClientInstance
{
    
public:
    uintptr_t** vtable;
public: 
    static inline ClientInstance* instance = nullptr;
    static ClientInstance* get() {
        return instance;
    }

    LocalPlayer* getLocalPlayer();
    Minecraft* getMinecraft();
};

