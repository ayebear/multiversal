// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "level.h"
#include <sstream>
#include "configfile.h"
#include "events.h"
#include "gameevents.h"

Level::Level(const std::string& levelDir, TileMap& tiles):
    levelDir(levelDir),
    tiles(tiles),
    currentLevel(1)
{
}

bool Level::load(int level)
{
    // Load configuration
    std::string filename(levelDir + std::to_string(level) + ".cfg");
    cfg::File config(filename);
    if (!config)
        return false;
    tiles.resize(config("width").toInt(), config("height").toInt());
    config.useSection("Layers");

    // Load layer data
    int layerNum = 1;
    sf::Vector2u startTilePos;
    for (auto& layerName: {"layer1", "layer2"})
    {
        int y = 0;
        tiles.useLayer(layerNum++);
        for (auto& visualTiles: config(layerName))
        {
            int x = 0;
            std::istringstream data(visualTiles.toString());
            int tileId = 0;
            while (data >> tileId)
            {
                // This is for the player starting position
                // TODO: Add proper logical/visual layers
                if (tileId == 9)
                {
                    startTilePos.x = x;
                    startTilePos.y = y;
                    tileId = 0;
                }
                tiles.set(x, y, tileId);
                ++x;
            }
            ++y;
        }
    }

    // Send player starting position event
    auto tileSize = tiles.getTileSize();
    sf::Vector2f startPos(startTilePos.x * tileSize.x, startTilePos.y * tileSize.y);
    Events::send(PlayerPosition{startPos, startTilePos});

    tiles.useLayer(1);
    currentLevel = level;

    return true;
}

bool Level::loadNext()
{
    return load(currentLevel + 1);
}
