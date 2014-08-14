// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "rendersystem.h"
#include "components.h"
#include "tilemap.h"
#include "camera.h"
#include "magicwindow.h"
#include "views.h"

RenderSystem::RenderSystem(ocs::ObjectManager& entities, TileMap& tiles, sf::RenderWindow& window, Camera& camera, MagicWindow& magicWindow):
    entities(entities),
    tiles(tiles),
    window(window),
    camera(camera),
    magicWindow(magicWindow)
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
    tiles.drawLayer(window, 1);
    //window.draw(tiles);

    // Draw the magic window
    auto& texture = magicWindow.getTexture();
    texture.clear(sf::Color::Transparent);
    auto windowViewPos = getViewPos(camera.getView("game"));
    magicWindow.setView(camera.accessView("background"), windowViewPos);
    texture.draw(sprites("background2"));
    magicWindow.setView(camera.accessView("game"), windowViewPos);
    tiles.drawLayer(texture, 2);
    texture.display();
    window.draw(magicWindow);

    // Draw sprites
    for (auto& sprite: entities.getComponentArray<Components::Sprite>())
        window.draw(sprite.sprite);

    // Draw animated sprites
    for (auto& animSprite: entities.getComponentArray<Components::AnimSprite>())
        window.draw(animSprite.sprite);

    window.display();
}
