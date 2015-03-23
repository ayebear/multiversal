// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "basestate.h"
#include "game.h"

class GameResources;

/*
The state for the playable game, which just holds an instance of the Game class.
*/
class GameState: public BaseState
{
    public:
        GameState(GameResources& resources);

        void onStart();
        void handleEvents();
        void update();
        void draw() {} // Update() calls the render system to draw

    private:
        GameResources& resources;
        Game game;
};

#endif
