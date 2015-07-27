// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <SFML/System/Vector2.hpp>
#include "configfile.h"
#include "es/world.h"

namespace ng { class TileMap; }
class TileMapData;
class TileMapChanger;
class MagicWindow;

/*
This class loads data from level files:
    Tile map layers
    Game instance

Note that it does not store this data, only populates other objects with the data.

The current level file format looks like this:
    height = 12
    width = 32
    version = 1
    name = "Some level"

    [0: Real]
    visual = { ... }
    logical = { ... }

    [1: Alternate]
    visual = { ... }
    logical = { ... }

    [Entities]
    name:Type = {
        "Component 321 100"
    }
*/
class Level
{
    public:

        Level(TileMapData& tileMapData, ng::TileMap& tileMap, TileMapChanger& tileMapChanger, es::World& world, MagicWindow& magicWindow);

        // Loads a level file
        bool loadFromFile(const std::string& filename);
        void loadFromString(const std::string& data);

        // Saves a level file
        bool saveToFile(const std::string& filename) const;
        void saveToString(std::string& data) const;

        // Resets tilemaps and world
        void clear();

        // Returns the name of the level
        const std::string& getName() const;

        // Loads world from a section in a config file
        static void loadEntities(cfg::File::Section& section, es::World& world);

    private:

        // Loading levels
        void load(cfg::File& config);
        void loadLogicalLayer(cfg::File& config, int layer);
        void loadVisualLayer(cfg::File& config, int layer);
        void loadTileMap(cfg::File& config);
        void loadEntities(cfg::File& config);

        // Saving levels
        void save(cfg::File& config) const;
        void saveTileMap(cfg::File& config) const;
        void saveEntities(cfg::File& config) const;

        static const cfg::File::ConfigMap defaultOptions;

        TileMapData& tileMapData; // Logical tile map
        ng::TileMap& tileMap; // Visual tile map
        TileMapChanger& tileMapChanger;
        es::World& world;
        MagicWindow& magicWindow;

        std::string name;
};

#endif
