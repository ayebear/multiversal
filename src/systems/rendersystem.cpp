// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "rendersystem.h"
#include "tilemap.h"
#include "camera.h"
#include "magicwindow.h"
#include "views.h"

RenderSystem::RenderSystem(ocs::ObjectManager& entities, TileMap& tileMap, sf::RenderWindow& window, Camera& camera, MagicWindow& magicWindow):
    entities(entities),
    tileMap(tileMap),
    window(window),
    camera(camera),
    magicWindow(magicWindow),
    texture(magicWindow.getTexture())
{
    // Load the background images
    sprites.loadFromConfig("data/config/sprites.cfg");
}

void RenderSystem::update()
{
    window.clear();

    // Draw the real world
    window.setView(camera.getView("background"));
    window.draw(sprites("background"));
    window.setView(camera.getView("game"));
    tileMap.drawLayer(window, 0);
    //window.draw(tileMap);

    // Draw the magic window
    //auto& texture = magicWindow.getTexture();
    texture.clear(sf::Color::Transparent);
    auto windowViewPos = getViewPos(camera.getView("game"));
    magicWindow.setView(camera.accessView("background"), windowViewPos);
    texture.draw(sprites("background2"));
    magicWindow.setView(camera.accessView("game"), windowViewPos);
    tileMap.drawLayer(texture, 1);

    // Note: May add real z-index in the future, for now this should work good enough

    // Draw sprites
    for (auto& sprite: entities.getComponentArray<Components::Sprite>())
        drawSprite(sprite);

    // Draw animated sprites
    for (auto& animSprite: entities.getComponentArray<Components::AnimSprite>())
        drawSprite(animSprite);

    // Finish drawing the render texture for the magic window
    texture.display();
    window.draw(magicWindow);

    // Draw sprites
    for (auto& sprite: entities.getComponentArray<Components::Sprite>())
        drawSprite(sprite, true);

    // Draw animated sprites
    for (auto& animSprite: entities.getComponentArray<Components::AnimSprite>())
        drawSprite(animSprite, true);

    window.display();
}
