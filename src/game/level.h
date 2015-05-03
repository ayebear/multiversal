// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <SFML/System/Vector2.hpp>
#include "configfile.h"
#include "OCS/Misc/Config.hpp"

namespace ocs { class ObjectManager; }
namespace ng { class TileMap; }
class TileMapData;
class TileMapChanger;
class MagicWindow;

/*
This class loads data from level files:
    Tile map layers
    Game objects

Note that it does not store this data, only populates other objects with the data.

The current level file format looks like this:
    height = 12
    width = 32
    version = 1

    [Real]
    visual = { ... }
    logical = { ... }

    [Alternate]
    visual = { ... }
    logical = { ... }

    [Objects]
    name:Type = {
        "Component 321 100"
    }
*/
class Level
{
    public:

        using ObjectNameMap = std::map<std::string, ocs::ID>;

        Level(TileMapData& tileMapData, ng::TileMap& tileMap, TileMapChanger& tileMapChanger, ocs::ObjectManager& objects, MagicWindow& magicWindow);

        // Loads a level file
        bool loadFromFile(const std::string& filename);
        void loadFromString(const std::string& data);

        // Saves a level file
        bool saveToFile(const std::string& filename) const;
        void saveToString(std::string& data) const;

        // Things that should be in OCS ---------------------------------------

        // Returns an object ID from an object's name
        ocs::ID getObjectIdFromName(const std::string& name) const;

        // Registers an object name to an ID
        void registerObjectName(ocs::ID objectId, const std::string& name);

        // Removes an object from the index
        void unregisterObjectName(const std::string& name);

        // --------------------------------------------------------------------

        // Loads objects from a section in a config file
        void loadObjects(cfg::File::Section& section, ocs::ObjectManager& objects, ObjectNameMap& objectNames) const;

        // Resets tilemaps and objects
        void clear();

    private:

        // Loading levels
        void load(cfg::File& config);
        void loadLogicalLayer(cfg::File& config, int layer);
        void loadVisualLayer(cfg::File& config, int layer);
        void loadTileMap(cfg::File& config);
        void loadObjects(cfg::File& config);

        // Saving levels
        void save(cfg::File& config) const;
        void saveTileMap(cfg::File& config) const;
        void saveObjects(cfg::File& config) const;

        static const cfg::File::ConfigMap defaultOptions;

        TileMapData& tileMapData; // Logical tile map
        ng::TileMap& tileMap; // Visual tile map
        TileMapChanger& tileMapChanger;
        ocs::ObjectManager& objects;
        MagicWindow& magicWindow;

        ObjectNameMap objectNamesToIds;
};

#endif
