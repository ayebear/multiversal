// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "tilemapdata.h"
#include "tilemap.h"
#include "tilemapchanger.h"
#include "camera.h"
#include "level.h"
#include "levelloader.h"
#include "magicwindow.h"
#include "OCS/Objects.hpp"
#include "matrix.h"
#include "systemcontainer.h"
#include "actionhandler.h"

/*
Contains the class instances used by the game.
*/
struct GameWorld
{
    GameWorld(sf::RenderWindow& window);

    ng::ActionHandler actions;
    TileMapData tileMapData;
    TileMap tileMap;
    TileMapChanger tileMapChanger;
    Camera camera;
    Level level;
    LevelLoader levelLoader;
    MagicWindow magicWindow;
    ocs::ObjectManager entities;
    es::SystemContainer systems;
};

#endif
