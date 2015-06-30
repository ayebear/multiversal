// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef TILESYSTEM_H
#define TILESYSTEM_H

#include "es/system.h"

class TileMapData;
namespace es
{
    class Entity;
    class World;
}

/*
This class handles the action key presses for the special tiles.
It then proxies the events so other things can handle what happened.
*/
class TileSystem: public es::System
{
    public:
        TileSystem(es::World& world, TileMapData& tileMapData);
        void update(float dt);

    private:
        void handleExitTile();

        es::World& world;
        TileMapData& tileMapData;
};

#endif
