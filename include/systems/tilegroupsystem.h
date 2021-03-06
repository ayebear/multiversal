// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef TILEGROUPSYSTEM_H
#define TILEGROUPSYSTEM_H

#include "es/system.h"

class TileMapChanger;
namespace es
{
    class Entity;
    class World;
}

/*
Handles changing the states of the tile IDs in TileGroup components.
Only does this when the State component changes.
*/
class TileGroupSystem: public es::System
{
    public:
        TileGroupSystem(TileMapChanger& tileMapChanger, es::World& world);
        void initialize();
        void update(float dt);

    private:
        TileMapChanger& tileMapChanger;
        es::World& world;
};

#endif
