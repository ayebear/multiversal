// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SFML/Graphics.hpp>
#include "OCS/Objects.hpp"
#include "components.h"
#include "nage/graphics/spriteloader.h"
#include "system.h"

namespace ng
{
    class TileMap;
    class Camera;
}

class MagicWindow;

/*
This class will handle all of the drawing to the window, so that the game state doesn't need to mess with any of that...
It will render all of the drawable objects like the tile map, and drawable components (sprites, animated sprites, etc.)
*/
class RenderSystem: public es::System
{
    public:
        RenderSystem(ocs::ObjectManager& entities, ng::TileMap& tileMap, sf::RenderWindow& window, ng::Camera& camera, MagicWindow& magicWindow);
        void update(float dt);

    private:
        // Determines if an object is in the alternate world
        bool inAltWorld(ocs::ID id) const;

        // Draws laser beams
        void drawLasers(int layer);

        template <class SpriteType>
        void drawSprite(const SpriteType& sprite, bool onTop = false);

        // References to various things to draw
        ocs::ObjectManager& entities;
        ng::TileMap& tileMap;
        sf::RenderWindow& window;
        ng::Camera& camera;
        MagicWindow& magicWindow;
        sf::RenderTexture* texture;

        ng::SpriteLoader sprites;
};

// Draws either an animated sprite or regular sprite component
template <class SpriteType>
void RenderSystem::drawSprite(const SpriteType& sprite, bool onTop)
{
    bool hasOnTop = entities.hasComponents<Components::DrawOnTop>(sprite.getOwnerID());
    if (hasOnTop == onTop)
    {
        if (inAltWorld(sprite.getOwnerID()))
            texture->draw(sprite.sprite);
        else
            window.draw(sprite.sprite);
    }
}

#endif
