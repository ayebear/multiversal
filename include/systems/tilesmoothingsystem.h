// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef TILESMOOTHINGSYSTEM_H
#define TILESMOOTHINGSYSTEM_H

#include "es/system.h"
#include "configfile.h"

class TileMapData;
namespace ng { class TileMap; }
namespace es { class World; }

/*
This class handles generating "smooth" connected tiles.
Handles events for partial tile updates (for use with the level editor).
*/
class TileSmoothingSystem: public es::System
{
    public:
        TileSmoothingSystem(es::World& world, const TileMapData& tileMapData, ng::TileMap& smoothTileMap);
        void initialize();
        void update(float dt);

    private:
        void updateTile(int layer, int x, int y);
        char getKey(int layer, int x, int y) const;

        cfg::File mappings;
        es::World& world;
        const TileMapData& tileMapData;
        ng::TileMap& smoothTileMap;
};

#endif
