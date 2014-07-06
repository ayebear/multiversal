// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "level.h"
#include <sstream>
#include "configfile.h"

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
    tiles.useLayer(1);

    // Load layer data
    config.useSection("Layers");
    int y = 0;
    for (auto& visualTiles: config("visual"))
    {
        int x = 0;
        std::istringstream data(visualTiles.toString());
        int tileId = 0;
        while (data >> tileId)
        {
            tiles.set(x, y, tileId);
            ++x;
        }
        ++y;
    }

    currentLevel = level;

    return true;
}

bool Level::loadNext()
{
    return load(currentLevel + 1);
}
