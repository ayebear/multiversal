// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "tilemapdata.h"
#include "nage/graphics/tilemap.h"
#include "tilemapchanger.h"
#include "nage/graphics/camera.h"
#include "level.h"
#include "levelloader.h"
#include "magicwindow.h"
#include "nage/misc/matrix.h"
#include "es/systemcontainer.h"
#include "nage/actions/actionhandler.h"

class GameSaveHandler;

/*
Contains the class instances used by the game.
*/
struct GameInstance
{
    GameInstance(sf::RenderWindow& window, GameSaveHandler& gameSave);

    ng::ActionHandler actions;
    TileMapData tileMapData;
    ng::TileMap tileMap;
    ng::TileMap smoothTileMap;
    TileMapChanger tileMapChanger;
    ng::Camera camera;
    Level level;
    LevelLoader levelLoader;
    MagicWindow magicWindow;
    es::World world;
    es::SystemContainer systems;
};

#endif
