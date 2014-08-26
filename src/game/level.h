// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include "tilemap.h"

class Level
{
    public:
        Level(const std::string& levelDir, TileMap& tiles);
        bool load(int level);
        bool loadNext();

    private:
        void sendStartPosition(sf::Vector2f& pos);

        std::string levelDir;
        TileMap& tiles;
        int currentLevel;
};

#endif
