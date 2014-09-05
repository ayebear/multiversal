// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "level.h"
#include <sstream>
#include "events.h"
#include "gameevents.h"
#include "tilemap.h"
#include "components.h"

const cfg::File::ConfigMap Level::defaultOptions = {
    {"",{
        {"width", cfg::makeOption(32, 0)},
        {"height", cfg::makeOption(12, 0)}
        }
    }
};

Level::Level(const std::string& levelDir, TileMapData& tileMapData, TileMap& tileMap, ocs::ObjectManager& entities):
    levelDir(levelDir),
    tileMapData(tileMapData),
    tileMap(tileMap),
    entities(entities),
    currentLevel(1)
{
}

bool Level::load(int level)
{
    // Load configuration
    std::string filename(levelDir + std::to_string(level) + ".cfg");
    cfg::File config(filename, defaultOptions);
    if (!config)
        return false;

    loadTileMap(config);
    loadObjects(config);

    currentLevel = level;
    return true;
}

void Level::loadTileMap(cfg::File& config)
{
    // Resize tile maps
    unsigned width = config("width").toInt();
    unsigned height = config("height").toInt();
    tileMap.resize(width, height);
    tileMapData.resize(width, height);

    // Load layer data
    sf::Vector2u startTilePos;
    int currentLayer = 0;
    for (auto& section: config)
    {
        if (section.first == "Real")
            currentLayer = 0;
        else if (section.first == "Alternate")
            currentLayer = 1;
        else
            continue;

        tileMap.useLayer(currentLayer);
        tileMapData.useLayer(currentLayer);

        int y = 0;
        config.useSection(section.first);

        // TODO: Make this less redundant...

        for (auto& tiles: config("logical"))
        {
            int x = 0;
            std::istringstream data(tiles.toString());
            int logicalId = 0;
            while (data >> logicalId)
            {
                tileMapData(x, y).logicalId = logicalId;
                if (logicalId == 9)
                {
                    startTilePos.x = x;
                    startTilePos.y = y;
                }
                ++x;
            }
            ++y;
        }

        y = 0;
        for (auto& tiles: config("visual"))
        {
            int x = 0;
            std::istringstream data(tiles.toString());
            int visualId = 0;
            while (data >> visualId)
            {
                tileMapData(x, y).visualId = visualId;
                tileMap.set(x, y, visualId);
                ++x;
            }
            ++y;
        }

        y = 0;
        for (auto& tiles: config("collision"))
        {
            int x = 0;
            std::istringstream data(tiles.toString());
            bool collidable = false;
            while (data >> collidable)
            {
                tileMapData(x, y).collidable = collidable;
                ++x;
            }
            ++y;
        }

        y = 0;
        for (auto& tiles: config("state"))
        {
            int x = 0;
            std::istringstream data(tiles.toString());
            bool state = false;
            while (data >> state)
            {
                tileMapData(x, y).state = state;
                ++x;
            }
            ++y;
        }
    }

    // Send player starting position event
    auto tileSize = tileMap.getTileSize();
    sf::Vector2f startPos(startTilePos.x * tileSize.x, startTilePos.y * tileSize.y);
    Events::send(PlayerPosition{startPos, startTilePos});

    tileMap.useLayer(0);
}

void Level::loadObjects(cfg::File& config)
{
    // TODO: Have another tile-like layer for creating objects
    // There could be just the names of the objects, or some IDs that get the names from a mapping file
    // Will need some sort of parameters for the objects, these will need to be hard-coded
    // Another option could be to have the objects in the tile map and set their positions to that
    for (auto& option: config.getSection("Objects"))
    {
        for (auto& object: option.second)
        {
            auto id = entities.createObject(option.first);
            if (option.first == "Box" || option.first == "Box2")
            {
                auto pos = entities.getComponent<Components::Position>(id);
                if (pos)
                {
                    std::istringstream data(object.toString());
                    int x, y;
                    if (data >> x >> y)
                    {
                        pos->x = x;
                        pos->y = y;
                    }
                }
            }
        }
    }
}

bool Level::loadNext()
{
    return load(currentLevel + 1);
}
