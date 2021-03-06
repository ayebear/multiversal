// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "rendersystem.h"
#include "nage/graphics/tilemap.h"
#include "nage/graphics/camera.h"
#include "magicwindow.h"
#include "nage/graphics/views.h"
#include "lasercomponent.h"
#include "level.h"
#include "gamesavehandler.h"
#include <iostream>

RenderSystem::RenderSystem(es::World& world, ng::TileMap& tileMap, ng::TileMap& smoothTileMap,
        sf::RenderWindow& window, ng::Camera& camera, MagicWindow& magicWindow, const Level& level,
        const GameSaveHandler& gameSave):
    world(world),
    tileMap(tileMap),
    smoothTileMap(smoothTileMap),
    window(window),
    camera(camera),
    magicWindow(magicWindow),
    level(level),
    gameSave(gameSave)
{
    // Load the background images
    sprites.loadFromConfig("data/config/sprites.cfg");

    // Setup sf::Text objects
    if (!font.loadFromFile("data/fonts/Ubuntu-B.ttf"))
    {
        std::cerr << "Error loading font file: 'data/fonts/Ubuntu-B.ttf'\n";
        exit(1);
    }
    levelNumberText.setFont(font);
    levelNumberText.setCharacterSize(32);
    levelNumberText.setColor(sf::Color::Black);
    levelNameText.setFont(font);
    levelNameText.setCharacterSize(32);
    levelNameText.setColor(sf::Color::Black);
}

void RenderSystem::initialize()
{
    // Vertically scale the background to the level and keep the same aspect ratio
    float targetHeight = camera.accessView("background").getSize().y;
    for (const auto& name: {"background", "background2"})
    {
        auto& sprite = sprites(name);
        sprite.setScale(1, 1);
        auto bounds = sprite.getLocalBounds();
        float scale = targetHeight / bounds.height;
        sprite.setScale(scale, scale);
    }

    // Update positions based on window view
    uiView = window.getDefaultView();
    auto uiViewSize = uiView.getSize();
    levelNumberText.setPosition(uiViewSize.x - 100, uiViewSize.y - 48);
    levelNameText.setPosition(8, uiViewSize.y - 48);

    // Update current level number
    levelNumberText.setString(std::to_string(gameSave.getCurrentLevel()) +
            '/' + std::to_string(GameSaveHandler::TOTAL_LEVELS));

    // Update current level name
    levelNameText.setString(level.getName());
}

void RenderSystem::update(float dt)
{
    window.clear(sf::Color(128, 128, 128));

    // Draw the real world
    window.setView(camera.getView("background"));
    window.draw(sprites("background"));
    window.setView(camera.getView("game"));
    tileMap.drawLayer(window, 0);
    smoothTileMap.drawLayer(window, 0);

    // Draw the magic window
    texture = &magicWindow.getRenderTexture();
    texture->clear(sf::Color(0, 128, 0));
    auto windowViewPos = ng::views::getViewPos(camera.getView("game"));
    magicWindow.setView(camera.accessView("background"), windowViewPos);
    texture->draw(sprites("background2"));
    magicWindow.setView(camera.accessView("game"), windowViewPos);
    tileMap.drawLayer(*texture, 1);
    smoothTileMap.drawLayer(*texture, 1);

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

    // Draw level name/number text
    window.setView(uiView);
    window.draw(levelNumberText);
    window.draw(levelNameText);

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
