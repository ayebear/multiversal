// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "level.h"
#include <sstream>
#include "events.h"
#include "gameevents.h"
#include "tilemapdata.h"
#include "nage/graphics/tilemap.h"
#include "tilemapchanger.h"
#include "components.h"
#include "logicaltiles.h"
#include "magicwindow.h"
#include "OCS/Objects.hpp"

const cfg::File::ConfigMap Level::defaultOptions = {
    {"", {
        {"width", cfg::makeOption(32, 0)},
        {"height", cfg::makeOption(12, 0)},
        {"version", cfg::makeOption(1, 0)}
        }
    }
};

Level::Level(TileMapData& tileMapData, ng::TileMap& tileMap, TileMapChanger& tileMapChanger, ocs::ObjectManager& objects, MagicWindow& magicWindow):
    tileMapData(tileMapData),
    tileMap(tileMap),
    tileMapChanger(tileMapChanger),
    objects(objects),
    magicWindow(magicWindow)
{
}

bool Level::loadFromFile(const std::string& filename)
{
    cfg::File config(filename, defaultOptions);
    bool status = config.getStatus();
    if (status)
    {
        load(config);
        std::cout << "Loaded level file: " << filename << "\n";
    }
    else
        std::cerr << "Error loading level file: " << filename << "\n";
    return status;
}

void Level::loadFromString(const std::string& data)
{
    cfg::File config(defaultOptions);
    config.loadFromString(data);
    load(config);
    std::cout << "Loaded new level from memory.\n";
}

bool Level::saveToFile(const std::string& filename) const
{
    // Save everything to a level file
    cfg::File config(defaultOptions);
    save(config);

    // Write the level file
    bool status = config.writeToFile(filename);
    if (status)
        std::cout << "Saved level file: " << filename << "\n";
    else
        std::cerr << "Error saving level file: " << filename << "\n";
    return status;
}

void Level::saveToString(std::string& data) const
{
    cfg::File config(defaultOptions);
    save(config);
    config.writeToString(data);
    std::cout << "Saved level to memory.\n";
}

void Level::load(cfg::File& config)
{
    // Load everything from the config file
    loadTileMap(config);
    loadObjects(config);

    // Reset window
    magicWindow.show(false);
    magicWindow.setSize();
}

void Level::save(cfg::File& config) const
{
    // Save everything to a level file in memory
    saveTileMap(config);
    saveObjects(config);
}

ocs::ID Level::getObjectIdFromName(const std::string& name) const
{
    ocs::ID id = -1;
    auto found = objectNamesToIds.find(name);
    if (found != objectNamesToIds.end())
        id = found->second;
    return id;
}

void Level::registerObjectName(ocs::ID objectId, const std::string& name)
{
    objectNamesToIds[name] = objectId;
}

void Level::loadObjects(cfg::File::Section& section, ocs::ObjectManager& objects, ObjectNameMap& objectNames, bool player) const
{
    objects.destroyAllObjects();
    objectNames.clear();
    bool playerCreated = false;

    // Create objects from level file
    for (auto& option: section)
    {
        // Extract object name and type
        auto names = strlib::split(option.first, ":");

        // Create an object (with the type if specified)
        ocs::ID id;
        if (names.size() == 2)
            id = objects.createObject(names.back());
        else
            id = objects.createObject();

        // Keep its unique name in a lookup table
        objectNames[names.front()] = id;
        if (names.front() == "player")
            playerCreated = true;

        // Update all specified components
        for (auto& componentStr: option.second)
        {
            const auto& compStr = componentStr.toString();
            if (compStr.empty())
                continue;
            auto separator = compStr.find(' ');
            if (separator != std::string::npos && separator > 0 && separator + 1 < compStr.size())
            {
                // Extract the component's name and data
                auto componentName = compStr.substr(0, separator);
                auto componentData = compStr.substr(separator + 1);
                std::cout << "Component: '" << componentName << "', Data: '" << componentData << "'\n";
                objects.updateComponentFromString(id, componentName, componentData);
            }
            else if (separator == std::string::npos)
            {
                std::cout << "Component: '" << compStr << "'\n";
                objects.updateComponentFromString(id, compStr, "");
            }
            else
                std::cerr << "ERROR: Cannot process '" << compStr << "'.\n";
        }
    }

    if (player && !playerCreated)
        createPlayer(objects, objectNames);
}

void Level::clear()
{
    tileMapChanger.clear();
    objects.destroyAllObjects();
    createPlayer(objects, objectNamesToIds);
}

void Level::createPlayer(ocs::ObjectManager& objects, ObjectNameMap& objectNames) const
{
    auto id = objects.createObject("Player");
    objectNames["player"] = id;
    const auto& tileSize = tileMap.getTileSize();
    objects.addComponents(id, Components::Position(tileSize.x, tileSize.y));
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
            if (tileMap.inBounds(x, y))
            {
                tileMapData(x, y).logicalId = logicalId;

                // Populate the logical to tile ID map
                if (logicalId != Tiles::None && logicalId != Tiles::Normal)
                    tileMapData[logicalId].push_back(tileMapData.getId(x, y));
            }
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
            if (tileMap.inBounds(x, y))
            {
                // Update the visual layer and graphical tile map
                tileMapData(x, y).visualId = visualId;
                tileMap.set(x, y, visualId);
            }
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
    for (auto& section: config)
    {
        // Determine which layer to use
        int currentLayer = strlib::fromString<int>(section.first);
        if (currentLayer < 0 || currentLayer > 1)
            continue;

        tileMap.useLayer(currentLayer);
        tileMapData.useLayer(currentLayer);
        config.useSection(section.first);

        loadLogicalLayer(config, currentLayer);
        loadVisualLayer(config, currentLayer);
    }

    // Derive the remaining layer data (this basically initializes the layers)
    tileMapData.deriveTiles();

    // Use the real world as the current layer
    tileMap.useLayer(0);
}

void Level::loadObjects(cfg::File& config)
{
    loadObjects(config.getSection("Objects"), objects, objectNamesToIds);
}

void Level::saveTileMap(cfg::File& config) const
{
    // Save size
    unsigned width = tileMapData.width();
    unsigned height = tileMapData.height();
    config.useSection();
    config("width") = width;
    config("height") = height;

    // Real/Alternate: logical, visual
    unsigned layer = 0;
    for (const auto& section: {"0: Real", "1: Alternate"})
    {
        tileMapData.useLayer(layer);
        config.useSection(section);
        for (unsigned y = 0; y < tileMapData.height(); ++y)
        {
            std::ostringstream logicalStream;
            std::ostringstream visualStream;
            for (unsigned x = 0; x < width; ++x)
            {
                auto& tile = tileMapData(x, y);
                logicalStream << tile.logicalId;
                visualStream << tile.visualId;
                if (x < width - 1)
                {
                    logicalStream << " ";
                    visualStream << " ";
                }
            }
            config("logical").push() = logicalStream.str();
            config("visual").push() = visualStream.str();
        }
        ++layer;
    }
}

void Level::saveObjects(cfg::File& config) const
{
    // TODO: Save with the prototype name, and only include different components
    config.useSection("Objects");
    for (const auto& object: objectNamesToIds)
    {
        // Serialize all of the components of this object
        auto& option = config(object.first);
        auto components = objects.serializeObject(object.second);
        std::sort(components.begin(), components.end());
        for (const auto& str: components)
            option.push() = str;
    }
}
