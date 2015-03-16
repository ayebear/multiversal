// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameworld.h"

GameWorld::GameWorld():
    tileMapChanger(tileMapData, tileMap),
    level(tileMapData, tileMap, tileMapChanger, entities, magicWindow),
    levelLoader(level, "data/levels/")
{
    actions.loadFromConfig("data/config/controls.cfg");
}
