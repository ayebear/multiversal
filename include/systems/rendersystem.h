// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SFML/Graphics.hpp>
#include "nage/graphics/spriteloader.h"
#include "es/system.h"
#include "es/world.h"
#include "components.h"
#include "inaltworld.h"

namespace ng
{
    class TileMap;
    class Camera;
}

class MagicWindow;
class Level;
class GameSaveHandler;

/*
This class will handle all of the drawing to the window, so that the game state doesn't need to mess with any of that...
It will render all of the drawable world like the tile map, and drawable components (sprites, animated sprites, etc.)
*/
class RenderSystem: public es::System
{
    public:
        RenderSystem(es::World& world, ng::TileMap& tileMap, ng::TileMap& smoothTileMap, sf::RenderWindow& window, ng::Camera& camera, MagicWindow& magicWindow, const Level& level, const GameSaveHandler& gameSave);
        void initialize();
        void update(float dt);

    private:
        // Draws laser beams
        void drawLasers(int layer);

        template <class SpriteType>
        void drawSprite(es::Entity& ent, bool onTop = false);

        // References to various things to draw
        es::World& world;
        ng::TileMap& tileMap;
        ng::TileMap& smoothTileMap;
        sf::RenderWindow& window;
        ng::Camera& camera;
        MagicWindow& magicWindow;
        const Level& level;
        const GameSaveHandler& gameSave;

        sf::RenderTexture* texture;
        ng::SpriteLoader sprites;

        sf::Font font;
        sf::View uiView;
        sf::Text levelNumberText;
        sf::Text levelNameText;
};

// Draws either an animated sprite or regular sprite component
template <class SpriteType>
void RenderSystem::drawSprite(es::Entity& ent, bool onTop)
{
    if (ent.has<DrawOnTop>() == onTop)
    {
        auto sprite = ent.get<SpriteType>();
        if (sprite && sprite->visible)
        {
            if (inAltWorld(ent))
                texture->draw(sprite->sprite);
            else
                window.draw(sprite->sprite);
        }
    }
}

#endif
