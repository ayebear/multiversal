// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilemapchanger.h"
#include "tilemap.h"
#include "tilemapdata.h"

TileMapChanger::TileMapChanger(TileMapData& tileMapData, TileMap& tileMap):
    tileMapData(tileMapData),
    tileMap(tileMap)
{
}

bool TileMapChanger::changeState(int tileId, bool state)
{
    if (tileMapData(tileId).state != state)
    {
        // Update the state and visual ID
        tileMapData(tileId).state = state;
        tileMapData.updateVisualId(tileId);
        tileMapData.updateCollision(tileId);

        // Update the graphical tile map with the new visual ID
        tileMap.set(tileMapData.getLayer(tileId), tileMapData.getX(tileId),
            tileMapData.getY(tileId), tileMapData(tileId).visualId);

        return true;
    }
    return false;
}

void TileMapChanger::toggleState(int tileId)
{
    changeState(tileId, !tileMapData(tileId).state);
}
