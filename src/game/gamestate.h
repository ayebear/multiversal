// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "basestate.h"
#include "tilemapdata.h"
#include "tilemap.h"
#include "camera.h"
#include "level.h"
#include "magicwindow.h"
#include "OCS/Objects.hpp"
#include "matrix.h"
#include "systemcontainer.h"

class GameObjects;

/*
The state for the playable game.
Handles input, updating, and drawing.
Currently has all of the game objects, systems, and level.
*/
class GameState: public BaseState
{
    public:
        GameState(GameObjects& objects);

        void handleEvents();
        void update();
        void draw() {} // Update() calls the render system to draw

    private:
        void bindStringsToComponents();

        GameObjects& objects;

        // TODO: Move to a world class
        TileMapData tileMapData;
        TileMap tileMap;
        Camera camera;
        Level level;
        MagicWindow magicWindow;
        ocs::ObjectManager entities;

        // Systems
        es::SystemContainer systems;
};

#endif
