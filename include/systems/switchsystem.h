// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SWITCHSYSTEM_H
#define SWITCHSYSTEM_H

#include "es/system.h"
#include "gameevents.h"

class TileMapData;
class TileMapChanger;
namespace es
{
    class Entity;
    class World;
}

/*
Handles the input and output of the switches on the tile map.
May make separate classes for the different switch types.
*/
class SwitchSystem: public es::System
{
    public:
        SwitchSystem(TileMapData& tileMapData, TileMapChanger& tileMapChanger, es::World& world);
        void initialize();
        void update(float dt);

    private:
        void flipSwitch(int tileId, bool state);

        using SwitchObjectMap = std::map<int, std::vector<std::string>>;
        SwitchObjectMap switchObjects;

        TileMapData& tileMapData;
        TileMapChanger& tileMapChanger;
        es::World& world;
};

#endif
