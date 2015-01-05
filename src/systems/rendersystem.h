// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SFML/Graphics.hpp>
#include "OCS/Objects.hpp"
#include "components.h"
#include "spriteloader.h"
#include "system.h"

class TileMap;
class Camera;
class MagicWindow;

/*
This class will handle all of the drawing to the window, so that the game state doesn't need to mess with any of that...
It will render all of the drawable objects like the tile map, and drawable components (sprites, animated sprites, etc.)
*/
class RenderSystem: public es::System
{
    public:
        RenderSystem(ocs::ObjectManager& entities, TileMap& tileMap, sf::RenderWindow& window, Camera& camera, MagicWindow& magicWindow);
        void update(float dt);

    private:
        // References to various things to draw
        // TODO: Maybe use events to send things to draw?
        ocs::ObjectManager& entities;
        TileMap& tileMap;
        sf::RenderWindow& window;
        Camera& camera;
        MagicWindow& magicWindow;
        sf::RenderTexture& texture;

        SpriteLoader sprites;

        // Draws either an animated sprite or regular sprite component
        template <class SpriteType>
        void drawSprite(const SpriteType& sprite, bool onTop = false)
        {
            bool hasOnTop = entities.hasComponents<Components::DrawOnTop>(sprite.getOwnerID());
            if (hasOnTop == onTop)
            {
                if (entities.hasComponents<Components::AltWorld>(sprite.getOwnerID()))
                    texture.draw(sprite.sprite);
                else
                    window.draw(sprite.sprite);
            }
        }
};

#endif
