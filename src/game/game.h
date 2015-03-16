// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "gameworld.h"

/*
Contains the game world, objects, and systems.
This is used for 3 things:
    The actual game
    To test levels from the level editor
    To store the current level for the level editor
*/
class Game
{
    public:
        Game(sf::RenderWindow& window);

        // Loads the first level
        void initialize();

        // Updates the systems and everything else
        void update(float dt);

        // Returns a reference to the world with all of the objects
        GameWorld& getWorld();

    private:
        sf::RenderWindow& window;

        GameWorld world;
};

#endif
