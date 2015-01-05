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
    auto& visualIds = logicalToVisual[tile.logicalId];
    if (visualIds.size() >= 2)
        tile.visualId = visualIds[tile.state];
}

void TileMapData::updateCollision(int id)
{
    updateCollision(operator()(id));
}

void TileMapData::updateCollision(Tile& tile)
{
    // Get the regular collision data based on the logical ID
    tile.collidable = logicalToCollision[tile.logicalId];

    // Handle the special tiles where their state affects collision
    if (stateChangesCollision.find(tile.logicalId) != stateChangesCollision.end())
        tile.collidable = (tile.state == tile.collidable);
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

void TileMapData::printTiles() const
{
    /*std::cout << "Tiles: ";
    for (int id: tilesWithObjectsOnTop)
        std::cout << id << " ";
    std::cout << "\n";*/
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

    // Load collision data for tile types
    for (auto& option: config.getSection("LogicalToCollision"))
    {
        int logicalId = std::stoi(option.first);
        logicalToCollision[logicalId] = option.second.toBool();
    }

    // Load visual IDs for tile types
    for (auto& option: config.getSection("LogicalWithStateToVisual"))
    {
        int logicalId = std::stoi(option.first);
        for (auto& visualIdOption: option.second)
            logicalToVisual[logicalId].push_back(visualIdOption.toInt());
        logicalToVisual[logicalId].resize(2);
    }

    // Load logical IDs that should change collision with their state
    for (auto& option: config("stateChangesCollision"))
        stateChangesCollision.insert(option.toInt());
}
