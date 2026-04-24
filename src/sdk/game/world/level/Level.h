#pragma once


#include <cstdint>
#include <vector>
#include <functional>

class Actor;
class Player;

class Level
{
public:
    uintptr_t **vtable;
public:
    std::vector<Actor*> getRuntimeActorList();
    std::vector<Player*> getPlayers();
    void forEachPlayer(std::function<bool(Player&)> callback);
}; 