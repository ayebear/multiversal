// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAME_H
#define GAME_H

#include "basestate.h"
#include "tilemap.h"
#include "camera.h"
#include "level.h"
#include "magicwindow.h"
#include "inputsystem.h"
#include "playersystem.h"
#include "physicssystem.h"
#include "carrysystem.h"
#include "camerasystem.h"
#include "rendersystem.h"
#include "OCS/Objects.hpp"

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

        // Maybe move these to a world class?
        TileMap tiles;
        Camera camera;
        Level level;
        MagicWindow magicWindow;
        ocs::ObjectManager entities;

        // Systems
        InputSystem input;
        PlayerSystem player;
        PhysicsSystem physics;
        CarrySystem carrySystem;
        CameraSystem cameraSystem;
        RenderSystem render;
};

#endif
