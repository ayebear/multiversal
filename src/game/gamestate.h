// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAME_H
#define GAME_H

#include "basestate.h"
#include "tilemap.h"
#include "camera.h"
#include "level.h"
#include "entity.h"
#include "physicssystem.h"

class GameObjects;

/*
This class manages the window and input for the game
*/
class GameState: public BaseState
{
    public:
        GameState(GameObjects& objects);
        ~GameState();

        void onStart();
        void handleEvents();
        void update();
        void draw();

    private:

        GameObjects& objects;
        bool hasFocus;

        TileMap tiles;
        Camera camera;
        Level level;
        EntityList entities;
        PhysicsSystem physics;

        // TODO: Move this to the level loader
        sf::Texture bgTexture; // Should use some kind of resource/texture manager
        sf::Sprite bgSprite;
};

#endif
