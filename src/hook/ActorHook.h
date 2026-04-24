#pragma once

#include <vector>

class Actor;

class ActorHook {
public:
    typedef void(__fastcall* normalTick_t)(Actor* self);
    static inline normalTick_t originalNormalTick = nullptr;
    static void __fastcall hookedNormalTick(Actor* self);

    static void init();
};