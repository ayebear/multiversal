// Copyright (C) 2014-2015 Eric Hebert (ayebear)
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

const int Level::TOTAL_LEVELS = 1;

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

Level::LoadStatus Level::load(int level)
{
    if (level == -1)
        level = currentLevel;

    // Don't load more levels than the game should have
    if (level > TOTAL_LEVELS)
        return LoadStatus::Finished;

    std::cout << "Loading level " << level << "...\n";

    // Load configuration
    std::string filename(levelDir + std::to_string(level) + ".cfg");
    cfg::File config(filename, defaultOptions);
    if (!config)
    {
        std::cout << "Error loading level.\n";
        return LoadStatus::Error;
    }

    // Load everything from the config file
    loadTileMap(config);
    loadObjects(config);

    // Reset window
    magicWindow.show(false);

    currentLevel = level;

    return LoadStatus::Success;
}

Level::LoadStatus Level::loadNext()
{
    return load(currentLevel + 1);
}

ocs::ID Level::getObjectIdFromName(const std::string& name) const
{
    ocs::ID id = -1;
    auto found = objectNamesToIds.find(name);
    if (found != objectNamesToIds.end())
        id = found->second;
    return id;
}

bool Level::update()
{
    bool loadedLeved = false;

    // Check if the next level should be loaded
    if (es::Events::exists<LoadNextLevelEvent>())
    {
        auto status = loadNext();
        if (status == LoadStatus::Finished)
            es::Events::send(GameFinishedEvent{});
        else if (status == LoadStatus::Success)
            loadedLeved = true;
    }

    // Send the map size to the camera system
    es::Events::clear<MapSizeEvent>();
    es::Events::send(MapSizeEvent{tileMap.getPixelSize()});
    return loadedLeved;
}

void Level::sendStartPosition(sf::Vector2u& pos)
{
    auto tileSize = tileMap.getTileSize();
    sf::Vector2f startPos(pos.x * tileSize.x + 12, pos.y * tileSize.y - 32);
    es::Events::send(PlayerPosition{startPos, pos});
}

void Level::loadLogicalLayer(cfg::File& config, int layer)
{
    sf::Vector2u startTilePos;
    int y = 0;
    for (auto& tiles: config("logical"))
    {
        int x = 0;
        auto values = strlib::split<int>(tiles, " ");
        for (int logicalId: values)
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
    // Send the player's starting position
    if (layer == 0)
        sendStartPosition(startTilePos);
}

void Level::loadVisualLayer(cfg::File& config, int layer)
{
    int y = 0;
    for (auto& tiles: config("visual"))
    {
        int x = 0;
        auto values = strlib::split<int>(tiles, " ");
        for (int visualId: values)
        {
            tileMapData(x, y).visualId = visualId;
            tileMap.set(x, y, visualId);
            ++x;
        }
        ++y;
    }
}

void Level::loadTileMap(cfg::File& config)
{
    // Resize tile maps
    unsigned width = config("width").toInt();
    unsigned height = config("height").toInt();
    tileMap.resize(width, height);
    tileMapData.resize(width, height);
    tileMapData.clearTileIds();

    // Load layer data
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

        loadLogicalLayer(config, currentLayer);
        loadVisualLayer(config, currentLayer);
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
        // Extract object name and type
        auto names = strlib::split(option.first, ":");
        if (names.size() == 2)
        {
            // Create an object with the type
            auto id = entities.createObject(names[1]);

            // Keep its unique name in a lookup table
            objectNamesToIds[names[0]] = id;

            // Update all specified components
            for (auto& componentStr: option.second)
            {
                auto& compStr = componentStr.toString();
                auto separator = compStr.find(' ');
                if (separator != std::string::npos && separator > 0 && separator + 1 < compStr.size())
                {
                    // Extract the component's name and data
                    auto componentName = compStr.substr(0, separator);
                    auto componentData = compStr.substr(separator + 1);
                    std::cout << "Component name: '" << componentName << "', Data: '" << componentData << "'\n";
                    entities.updateComponentFromString(id, componentName, componentData);
                }
            }
        }
    }
}

void Level::loadSwitches(cfg::File& config)
{
    SwitchMapEvent event;
    for (auto& option: config.getSection("SwitchObjects"))
    {
        int switchTileId = std::stoi(option.first);
        for (auto& element: option.second)
            event.switchObjects[switchTileId].push_back(element.toString());
    }
    es::Events::send(event);
}
