// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "level.h"
#include <sstream>
#include <iostream>
#include "es/events.h"
#include "gameevents.h"
#include "tilemapdata.h"
#include "nage/graphics/tilemap.h"
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

Level::Level(TileMapData& tileMapData, ng::TileMap& tileMap, TileMapChanger& tileMapChanger, es::World& world, MagicWindow& magicWindow):
    tileMapData(tileMapData),
    tileMap(tileMap),
    tileMapChanger(tileMapChanger),
    world(world),
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
    loadEntities(config);

    // Reset window
    magicWindow.show(false);
    magicWindow.setSize();
}

void Level::save(cfg::File& config) const
{
    // Save everything to a level file in memory
    saveTileMap(config);
    saveEntities(config);
}

void Level::clear()
{
    tileMapChanger.clear();
    world.clear();
}

void Level::loadEntities(cfg::File::Section& section, es::World& world)
{
    std::cout << "Start\n";

    world.clear();

    // Create world from level file
    for (auto& option: section)
    {
        // Extract entity name and type
        auto names = strlib::split(option.first, ":");
        names.resize(2);

        std::cout << "    " << names.back() << ": " << names.front() << "\n";

        // Create an entity (with the type if specified)
        auto ent = world.copy(names.back(), names.front());

        // Update all specified components
        for (auto& compOption: option.second)
            ent << compOption.toString();

        // Store prototype name
        if (!names.back().empty())
            ent.assign<Prototype>(names.back());
    }

    std::cout << "End\n";

    std::cout << "Contents:\n";
    for (auto ent: world.query())
    {
        std::cout << "    " << ent.getName() << "\n";
        for (const auto& comp: ent.serialize())
            std::cout << "        " << comp << "\n";
    }
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
        // Parse the layer ID from the section name
        int currentLayer = strlib::fromString<int>(section.first, -1);

        if (currentLayer == 0 || currentLayer == 1)
        {
            tileMap.useLayer(currentLayer);
            tileMapData.useLayer(currentLayer);
            config.useSection(section.first);

            loadLogicalLayer(config, currentLayer);
            loadVisualLayer(config, currentLayer);
        }
    }

    // Derive the remaining layer data (states and collision layers)
    tileMapData.deriveTiles();

    // Use the real world as the current layer
    tileMap.useLayer(0);
}

void Level::loadEntities(cfg::File& config)
{
    loadEntities(config.getSection("Entities"), world);
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
            config("logical") << logicalStream.str();
            config("visual") << visualStream.str();
        }
        ++layer;
    }
}

void Level::saveEntities(cfg::File& config) const
{
    // TODO: Save with the prototype name, and only include different components
    config.useSection("Entities");
    for (const auto& ent: world.query())
    {
        if (!ent.has<ExcludeFromLevel>())
        {
            // Serialize all of the components and store them in an options array
            auto comps = ent.serialize();
            std::sort(comps.begin(), comps.end());

            // Get prototype name if it exists, as well as the option name
            std::string optionName = ent.getName();
            std::string prototypeName;
            auto prototype = ent.get<Prototype>();
            if (prototype)
            {
                prototypeName = prototype->entityName;
                if (!prototypeName.empty())
                    optionName += ':' + prototypeName;
            }

            // Get the option array being written to
            auto& option = config(optionName);

            // Get the prototype entity
            auto prototypeEnt = es::World::prototypes.get(prototypeName);

            // Save the components to the option array
            for (const auto& str: comps)
            {
                std::string compName;
                es::unpack(str, compName);

                // Only save components different from the components in the prototype
                if (compName != "Prototype" && prototypeEnt.serialize(compName) != str)
                    option << str;
            }
        }
    }
}
