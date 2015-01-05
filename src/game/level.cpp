// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "level.h"
#include <sstream>
#include "events.h"
#include "gameevents.h"
#include "tilemapdata.h"
#include "tilemap.h"
#include "tilemapchanger.h"
#include "components.h"
#include "logicaltiles.h"
#include "magicwindow.h"

const cfg::File::ConfigMap Level::defaultOptions = {
    {"",{
        {"width", cfg::makeOption(32, 0)},
        {"height", cfg::makeOption(12, 0)}
        }
    }
};

Level::Level(const std::string& levelDir, TileMapData& tileMapData, TileMap& tileMap,
        TileMapChanger& tileMapChanger, ocs::ObjectManager& entities, MagicWindow& magicWindow):
    levelDir(levelDir),
    tileMapData(tileMapData),
    tileMap(tileMap),
    tileMapChanger(tileMapChanger),
    entities(entities),
    magicWindow(magicWindow),
    currentLevel(1)
{
}

bool Level::load(int level)
{
    if (level == -1)
        level = currentLevel;

    std::cout << "Loading level " << level << "...\n";

    // Load configuration
    std::string filename(levelDir + std::to_string(level) + ".cfg");
    cfg::File config(filename, defaultOptions);
    if (!config)
    {
        std::cout << "Error loading level.\n";
        return false;
    }

    // Load everything from the config file
    loadTileMap(config);
    loadObjects(config);

    // Reset window
    magicWindow.show(false);

    currentLevel = level;

    return true;
}

bool Level::loadNext()
{
    return load(currentLevel + 1);
}

void Level::update()
{
    if (es::Events::exists<LoadNextLevelEvent>())
        loadNext();

    // Send the map size to the camera system
    es::Events::clear<MapSizeEvent>();
    es::Events::send(MapSizeEvent{tileMap.getPixelSize()});
}

void Level::sendStartPosition(sf::Vector2u& pos)
{
    auto tileSize = tileMap.getTileSize();
    sf::Vector2f startPos(pos.x * tileSize.x + 12, pos.y * tileSize.y - 32);
    es::Events::send(PlayerPosition{startPos, pos});
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
        // Determine which dimension to use
        if (section.first == "Real")
            currentLayer = 0;
        else if (section.first == "Alternate")
            currentLayer = 1;
        else
            continue;

        tileMap.useLayer(currentLayer);
        tileMapData.useLayer(currentLayer);

        config.useSection(section.first);

        // Load the logical layer
        int y = 0;
        for (auto& tiles: config("logical"))
        {
            int x = 0;
            std::istringstream data(tiles.toString());
            int logicalId = 0;
            while (data >> logicalId)
            {
                tileMapData(x, y).logicalId = logicalId;

                // Populate the logical to tile ID map
                if (logicalId != Tiles::None && logicalId != Tiles::Normal)
                    tileMapData[logicalId].push_back(tileMapData.getId(x, y));

                if (logicalId == Tiles::Start)
                {
                    startTilePos.x = x;
                    startTilePos.y = y;
                }
                ++x;
            }
            ++y;
        }

        // Load the visual layer
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
    }

    // Derive the remaining layer data (this basically initializes the layers)
    tileMapData.deriveTiles();

    // Set any of the tiles' states to true if specified
    config.useSection();
    for (auto& option: config("trueStates"))
    {
        int tileId = option.toInt();
        tileMapChanger.changeState(tileId, true);
    }

    // Load the switch connections
    loadSwitches(config);

    // Send the player's starting position
    sendStartPosition(startTilePos);

    // Use the real world as the current layer
    tileMap.useLayer(0);
}

void Level::loadObjects(cfg::File& config)
{
    entities.destroyAllObjects();

    // Always make a player object
    entities.createObject("Player");

    // Create objects from level file
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

void Level::loadSwitches(cfg::File& config)
{
    SwitchMapEvent event;
    for (auto& option: config.getSection("Switches"))
    {
        int switchTileId = std::stoi(option.first);
        for (auto& element: option.second)
            event.switches[switchTileId].push_back(element.toInt());
    }
    es::Events::send(event);
}
