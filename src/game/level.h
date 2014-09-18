// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <SFML/System/Vector2.hpp>
#include "configfile.h"
#include "tilemapdata.h"
#include "OCS/Objects.hpp"

class TileMap;

/*
This class holds the information for a level, and loads everything into memory from a file.
*/
class Level
{
    public:
        Level(const std::string& levelDir, TileMapData& tileMapData, TileMap& tileMap, ocs::ObjectManager& entities);
        bool load(int level = -1);
        bool loadNext();
        void update();

    private:
        void sendStartPosition(sf::Vector2u& pos);
        void loadTileMap(cfg::File& config);
        void loadObjects(cfg::File& config);

        static const cfg::File::ConfigMap defaultOptions;

        std::string levelDir;
        TileMapData& tileMapData; // Logical tile map
        TileMap& tileMap; // Visual tile map
        ocs::ObjectManager& entities;
        int currentLevel;
};

#endif
