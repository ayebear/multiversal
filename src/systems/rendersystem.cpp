// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "rendersystem.h"
#include "nage/graphics/tilemap.h"
#include "nage/graphics/camera.h"
#include "magicwindow.h"
#include "nage/graphics/views.h"
#include "lasercomponent.h"

RenderSystem::RenderSystem(es::World& world, ng::TileMap& tileMap, sf::RenderWindow& window, ng::Camera& camera, MagicWindow& magicWindow):
    world(world),
    tileMap(tileMap),
    window(window),
    camera(camera),
    magicWindow(magicWindow)
{
    // Load the background images
    sprites.loadFromConfig("data/config/sprites.cfg");
}

void RenderSystem::initialize()
{
    // TODO: Correctly resize backgrounds to fit level
    /*auto targetSize = camera.accessView("background").getSize();
    for (const auto& name: {"background", "background2"})
    {
        auto& sprite = sprites(name);
        sprite.setScale(1, 1);
        auto bounds = sprite.getLocalBounds();
        sprite.setScale(targetSize.x / bounds.width, targetSize.y / bounds.height);
    }*/
}

void RenderSystem::update(float dt)
{
    window.clear(sf::Color(128, 128, 128));

    // Draw the real world
    window.setView(camera.getView("background"));
    window.draw(sprites("background"));
    window.setView(camera.getView("game"));
    tileMap.drawLayer(window, 0);

    // Draw the magic window
    texture = &magicWindow.getRenderTexture();
    texture->clear(sf::Color(0, 128, 0));
    auto windowViewPos = ng::views::getViewPos(camera.getView("game"));
    magicWindow.setView(camera.accessView("background"), windowViewPos);
    texture->draw(sprites("background2"));
    magicWindow.setView(camera.accessView("game"), windowViewPos);
    tileMap.drawLayer(*texture, 1);

    // TODO: Add z-indexing with z-index components that get sorted
    // TODO: Make lasers, sprites, and animated sprites more easily drawable

    auto spriteEnts = world.query<Sprite>();
    auto animSpriteEnts = world.query<AnimSprite>();

    // Draw sprites
    for (auto& ent: spriteEnts)
        drawSprite<Sprite>(ent);

    // Draw animated sprites
    for (auto& ent: animSpriteEnts)
        drawSprite<AnimSprite>(ent);

    // Draw laser beams
    drawLasers(1);

    // Finish drawing the render texture for the magic window
    texture->display();
    window.draw(magicWindow);

    // Draw animated sprites
    for (auto& ent: animSpriteEnts)
        drawSprite<AnimSprite>(ent, true);

    // Draw sprites
    for (auto& ent: spriteEnts)
        drawSprite<Sprite>(ent, true);

    // Draw laser beams
    drawLasers(0);

    window.display();
}

void RenderSystem::drawLasers(int layer)
{
    for (auto& laser: world.getComponents<Laser>())
    {
        for (unsigned i = 0; i < laser.beams.size() && i < laser.beamCount; ++i)
        {
            if (laser.beams[i].layer == layer)
            {
                if (layer)
                    texture->draw(laser.beams[i].sprite);
                else
                    window.draw(laser.beams[i].sprite);
            }
        }
    }
}
