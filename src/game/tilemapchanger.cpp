// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilemapchanger.h"
#include "nage/graphics/tilemap.h"
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
        // Update the state, visual ID, and collision
        tileMapData(tileId).state = state;
        tileMapData.updateVisualId(tileId);
        tileMapData.updateCollision(tileId);
        updateVisualTile(tileId);
        stateChanged = true;
    }
    return stateChanged;
}

void TileMapChanger::toggleState(int tileId)
{
    changeState(tileId, !tileMapData(tileId).state);
}

void TileMapChanger::updateVisualTile(int tileId)
{
    // Update the graphical tile map with the new visual ID
    tileMap.set(tileMapData.getLayer(tileId), tileMapData.getX(tileId),
        tileMapData.getY(tileId), tileMapData(tileId).visualId);
}
