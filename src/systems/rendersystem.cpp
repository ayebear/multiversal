// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "rendersystem.h"
#include "tilemap.h"
#include "camera.h"
#include "magicwindow.h"
#include "views.h"
#include "lasercomponent.h"

RenderSystem::RenderSystem(ocs::ObjectManager& entities, TileMap& tileMap, sf::RenderWindow& window, Camera& camera, MagicWindow& magicWindow):
    entities(entities),
    tileMap(tileMap),
    window(window),
    camera(camera),
    magicWindow(magicWindow)
{
    // Load the background images
    sprites.loadFromConfig("data/config/sprites.cfg");
}

void RenderSystem::update(float dt)
{
    window.clear();

    // Draw the real world
    window.setView(camera.getView("background"));
    window.draw(sprites("background"));
    window.setView(camera.getView("game"));
    tileMap.drawLayer(window, 0);

    // Draw the magic window
    texture = &magicWindow.getRenderTexture();
    texture->clear(sf::Color::Transparent);
    auto windowViewPos = getViewPos(camera.getView("game"));
    magicWindow.setView(camera.accessView("background"), windowViewPos);
    texture->draw(sprites("background2"));
    magicWindow.setView(camera.accessView("game"), windowViewPos);
    tileMap.drawLayer(*texture, 1);

    // TODO: Add z-indexing with z-index components that get sorted

    // Draw sprites
    for (auto& sprite: entities.getComponentArray<Components::Sprite>())
        drawSprite(sprite);

    // Draw animated sprites
    for (auto& animSprite: entities.getComponentArray<Components::AnimSprite>())
        drawSprite(animSprite);

    // Draw laser beams
    drawLasers(1);

    // Finish drawing the render texture for the magic window
    texture->display();
    window.draw(magicWindow);

    // Draw animated sprites
    for (auto& animSprite: entities.getComponentArray<Components::AnimSprite>())
        drawSprite(animSprite, true);

    // Draw sprites
    for (auto& sprite: entities.getComponentArray<Components::Sprite>())
        drawSprite(sprite, true);

    // Draw laser beams
    drawLasers(0);

    window.display();
}

bool RenderSystem::inAltWorld(ocs::ID id) const
{
    // Check both the AltWorld and TilePosition components
    bool altWorld = entities.hasComponents<Components::AltWorld>(id);
    if (!altWorld)
    {
        auto tilePos = entities.getComponent<Components::TilePosition>(id);
        if (tilePos)
            altWorld = (tilePos->layer != 0);
    }
    return altWorld;
}

void RenderSystem::drawLasers(int layer)
{
    for (auto& laser: entities.getComponentArray<Components::Laser>())
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
