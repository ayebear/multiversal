// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilemapdata.h"
#include "configfile.h"
#include <iostream>

TileMapData::TileMapData():
    currentLayer(0)
{
    loadTileInfo();
}

Tile& TileMapData::operator()(int layer, unsigned x, unsigned y)
{
    return tiles[layer](x, y);
}

Tile& TileMapData::operator()(unsigned x, unsigned y)
{
    return tiles[currentLayer](x, y);
}

Tile& TileMapData::operator()(int id)
{
    return tiles[getLayer(id)](getX(id), getY(id));
}

void TileMapData::resize(unsigned width, unsigned height, bool preserve)
{
    for (auto& layer: tiles)
        layer.resize(width, height, preserve);
}

void TileMapData::useLayer(int layer)
{
    currentLayer = layer;
}

int TileMapData::getId(unsigned x, unsigned y) const
{
    return getId(currentLayer, x, y);
}

int TileMapData::getId(int layer, unsigned x, unsigned y) const
{
    return (layer * tiles[layer].size() + y * tiles[layer].width() + x);
}

void TileMapData::deriveTiles()
{
    for (auto& layer: tiles)
    {
        for (auto& tile: layer)
        {
            // Set the state to false and update the collision
            tile.state = false;
            updateCollision(tile);
        }
    }
}

void TileMapData::updateVisualId(int id)
{
    auto& tile = operator()(id);
    auto& tileInfo = logicalToInfo[tile.logicalId];
    if (tileInfo.stateToVisualUsed)
        tile.visualId = tileInfo.stateToVisual[tile.state];
}

void TileMapData::updateCollision(int id)
{
    updateCollision(operator()(id));
}

void TileMapData::updateCollision(Tile& tile)
{
    // Get the normal/laser collision data based on the logical ID
    auto& tileInfo = logicalToInfo[tile.logicalId];
    tile.collidable = tileInfo.collision[TileInfo::Collision + tile.state];
    tile.blocksLaser = tileInfo.collision[TileInfo::LaserCollision + tile.state];
}

void TileMapData::addTile(int id)
{
    tilesWithObjectsOnTop.insert(id);
}

bool TileMapData::findTile(int id) const
{
    return (tilesWithObjectsOnTop.find(id) != tilesWithObjectsOnTop.end());
}

void TileMapData::clearTiles()
{
    tilesWithObjectsOnTop.clear();
}

TileMapData::TileList& TileMapData::operator[](int logicalId)
{
    return tileIds[logicalId];
}

void TileMapData::clearTileIds()
{
    tileIds.clear();
}

int TileMapData::getLayer(int id) const
{
    return (id / tiles[currentLayer].size());
}

unsigned TileMapData::getX(int id) const
{
    return ((id % tiles[currentLayer].size()) % tiles[currentLayer].width());
}

unsigned TileMapData::getY(int id) const
{
    return ((id % tiles[currentLayer].size()) / tiles[currentLayer].width());
}

void TileMapData::loadTileInfo()
{
    cfg::File config("data/config/tile_info.cfg");

    // Load all of the information about certain logical tiles
    for (auto& option: config.getSection("TileInfo"))
    {
        int logicalId = strlib::fromString<int>(option.first);
        auto values = strlib::split<int>(option.second, " ");
        if (values.size() > TileInfo::LaserCollisionTrue)
        {
            auto& tileInfo = logicalToInfo[logicalId];

            // Extract collision properties
            unsigned index = 0;
            for (bool& col: tileInfo.collision)
                col = (values[index++] != 0);

            // Extract visual ID information
            if (values.size() > TileInfo::StateToVisualTrue)
            {
                tileInfo.stateToVisual[0] = values[TileInfo::StateToVisual];
                tileInfo.stateToVisual[1] = values[TileInfo::StateToVisualTrue];
                tileInfo.stateToVisualUsed = true;
            }
        }
    }
}

TileMapData::TileInfo::TileInfo():
    stateToVisualUsed(false)
{
    // Initialize arrays
    for (bool& c: collision)
        c = false;
    for (int& v: stateToVisual)
        v = 0;
}
