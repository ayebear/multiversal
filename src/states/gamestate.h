// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "basestate.h"

class GameObjects;
class Game;

/*
The state for the playable game, which just holds an instance of the Game class.
*/
class GameState: public BaseState
{
    public:
        GameState(GameObjects& objects, Game& game);

        void onStart();
        void handleEvents();
        void update();
        void draw() {} // Update() calls the render system to draw

    private:
        GameObjects& objects;
        Game& game;
};

#endif
