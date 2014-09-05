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
#include "inputsystem.h"
#include "playersystem.h"
#include "physicssystem.h"
#include "carrysystem.h"
#include "spritepositionsystem.h"
#include "camerasystem.h"
#include "rendersystem.h"
#include "OCS/Objects.hpp"
#include "matrix.h"

class GameObjects;

/*
This class manages the window and input for the game
*/
class GameState: public BaseState
{
    public:
        GameState(GameObjects& objects);

        void handleEvents();
        void update();
        void draw();

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
        // TODO: Use a system manager
        InputSystem inputSystem;
        PlayerSystem playerSystem;
        PhysicsSystem physicsSystem;
        CarrySystem carrySystem;
        SpritePositionSystem spritePositionSystem;
        CameraSystem cameraSystem;
        RenderSystem renderSystem;
};

#endif
