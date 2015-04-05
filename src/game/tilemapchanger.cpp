// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilemapchanger.h"
#include "nage/graphics/tilemap.h"
#include "tilemapdata.h"

TileMapChanger::TileMapChanger(TileMapData& tileMapData, ng::TileMap& tileMap):
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

void TileMapChanger::resize(int width, int height)
{
    if (width > 0 && height > 0)
    {
        tileMap.resize(width, height);
        tileMapData.resize(width, height);
        tileMapData.deriveTiles();

        // Update the visual tile map from the logical tile map
        apply([&](unsigned x, unsigned y)
        {
            tileMap.set(x, y, tileMapData(x, y).visualId);
        });
    }
}

void TileMapChanger::clear()
{
    // Reset all of the logical and visual tiles
    apply([&](unsigned x, unsigned y)
    {
        tileMap.set(x, y, 0);
        tileMapData(x, y).reset();
    });
}

void TileMapChanger::apply(FuncType callback)
{
    // Invoke callback for each tile in every layer
    for (unsigned layer = 0; layer <= 1; ++layer)
    {
        tileMap.useLayer(layer);
        tileMapData.useLayer(layer);
        unsigned height = tileMapData.height();
        unsigned width = tileMapData.width();
        for (unsigned y = 0; y < height; ++y)
        {
            for (unsigned x = 0; x < width; ++x)
                callback(x, y);
        }
    }
}
