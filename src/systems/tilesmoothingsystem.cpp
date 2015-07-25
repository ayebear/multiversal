// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilesmoothingsystem.h"
#include "nage/graphics/tilemap.h"
#include "es/world.h"
#include "logicaltiles.h"
#include "tilemapdata.h"
#include <iostream>
#include <algorithm>

TileSmoothingSystem::TileSmoothingSystem(es::World& world, TileMapData& tileMapData, ng::TileMap& smoothTileMap):
    mappings("data/config/smooth_mappings.cfg"),
    world(world),
    tileMapData(tileMapData),
    smoothTileMap(smoothTileMap)
{
}

void TileSmoothingSystem::initialize()
{
    // Setup smooth tilemap layer
    const auto size = tileMapData.size();
    smoothTileMap.resize(size.x * 2, size.y * 2);

    // Update all tiles
    const auto smoothSize = smoothTileMap.getMapSize();
    for (unsigned layer = 0; layer <= 1; ++layer)
    {
        for (unsigned y = 0; y < smoothSize.y; ++y)
        {
            for (unsigned x = 0; x < smoothSize.x; ++x)
                updateTile(layer, x, y);
        }
    }
}

void TileSmoothingSystem::update(float dt)
{
    // Handle partial updates from events (from a tile ID)
    // Would only update the 3x3 grid around this particular tile
}

void TileSmoothingSystem::updateTile(int layer, int x, int y)
{
    // Only generate tiles for platform tiles
    if (tileMapData(layer, x / 2, y / 2).logicalId != Tiles::Normal)
    {
        // Make it blank and skip
        smoothTileMap.set(layer, x, y, 14 + (layer * 18));
        return;
    }

    // Compute starting point of large tile
    const sf::Vector2i start{static_cast<int>(floor((x - 1) / 2.0)),
                             static_cast<int>(floor((y - 1) / 2.0))};

    // Build up a key from the surrounding 2x2 area of large tiles
    std::string key;
    key += getKey(layer, start.x, start.y);
    key += getKey(layer, start.x + 1, start.y);
    key += getKey(layer, start.x, start.y + 1);
    key += getKey(layer, start.x + 1, start.y + 1);

    // Note: Small tile is always an inner corner to 4 large tiles
    const unsigned smallTile = (x % 2) + ((y % 2) * 2);

    // Get the id from the mappings file
    int id = mappings(key, std::to_string(smallTile)).toInt();
    if (layer)
        id += 18;

    // Set tile image
    smoothTileMap.set(layer, x, y, id);
}

char TileSmoothingSystem::getKey(int layer, int x, int y) const
{
    unsigned platformTile = (tileMapData.inBounds(x, y) && tileMapData(layer, x, y).logicalId == Tiles::Normal);
    return (platformTile + '0');
}
