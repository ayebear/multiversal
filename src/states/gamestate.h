// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "nage/states/basestate.h"
#include "gameworld.h"

class GameResources;

/*
The state for the playable game.
Contains the game world, objects, and systems.
*/
class GameState: public ng::BaseState
{
    public:
        GameState(GameResources& resources);

        void onStart();
        void handleEvents();
        void update();
        void draw() {} // Update() calls the render system to draw

    private:
        GameResources& resources;
        GameWorld world;
};

#endif
