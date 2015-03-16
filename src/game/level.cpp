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
    {"", {
        {"width", cfg::makeOption(32, 0)},
        {"height", cfg::makeOption(12, 0)},
        {"version", cfg::makeOption(1, 0)}
        }
    }
};

Level::Level(TileMapData& tileMapData, TileMap& tileMap, TileMapChanger& tileMapChanger, ocs::ObjectManager& entities, MagicWindow& magicWindow):
    tileMapData(tileMapData),
    tileMap(tileMap),
    tileMapChanger(tileMapChanger),
    entities(entities),
    magicWindow(magicWindow)
{
}

bool Level::load(const std::string& filename)
{
    std::cout << "Loading level file \"" << filename << "\"...\n";
    cfg::File config(filename, defaultOptions);
    bool status = config.getStatus();
    if (status)
    {
        // Load everything from the config file
        loadTileMap(config);
        loadObjects(config);

        // Reset window
        magicWindow.show(false);
        magicWindow.setSize();
    }
    else
        std::cout << "Error loading level.\n";
    return status;
}

ocs::ID Level::getObjectIdFromName(const std::string& name) const
{
    ocs::ID id = -1;
    auto found = objectNamesToIds.find(name);
    if (found != objectNamesToIds.end())
        id = found->second;
    return id;
}

void Level::loadLogicalLayer(cfg::File& config, int layer)
{
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

            ++x;
        }
        ++y;
    }
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

            // Update the graphical tile map
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
