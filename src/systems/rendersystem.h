// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SFML/Graphics.hpp>
#include "OCS/Objects.hpp"
#include "spriteloader.h"

class TileMap;
class Camera;
class MagicWindow;

/*
This class will handle all of the drawing to the window, so that the game state doesn't need to mess with any of that...
It will render all of the drawable objects like the tile map, and drawable components (sprites, animated sprites, etc.)
*/
class RenderSystem
{
    public:
        RenderSystem(ocs::ObjectManager& entities, TileMap& tiles, sf::RenderWindow& window, Camera& camera, MagicWindow& magicWindow);
        void update();

    private:
        ocs::ObjectManager& entities;
        TileMap& tiles;
        sf::RenderWindow& window;
        Camera& camera;
        MagicWindow& magicWindow;

        SpriteLoader sprites;
};

#endif
