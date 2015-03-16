// Copyright (C) 2014-2015 Eric Hebert (ayebear)
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
    bool stateChanged = false;
    if (tileMapData(tileId).state != state)
    {
        tileMapData(tileId).state = state;
        updateTile(tileId);
        stateChanged = true;
    }
    return stateChanged;
}

void TileMapChanger::toggleState(int tileId)
{
    changeState(tileId, !tileMapData(tileId).state);
}

void TileMapChanger::updateTile(int tileId)
{
    // Update the state and visual ID
    tileMapData.updateVisualId(tileId);
    tileMapData.updateCollision(tileId);

    // Update the graphical tile map with the new visual ID
    tileMap.set(tileMapData.getLayer(tileId), tileMapData.getX(tileId),
        tileMapData.getY(tileId), tileMapData(tileId).visualId);
}
