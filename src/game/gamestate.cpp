// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamestate.h"
#include "gameobjects.h"
#include "player.h"
#include <iostream>

GameState::GameState(GameObjects& objects):
    objects(objects),
    level("data/levels/", tiles),
    physics(entities, tiles)
{
    hasFocus = true;

    // Load the tileset
    cfg::File tilesConfig("data/config/tiles.cfg", cfg::File::Warnings | cfg::File::Errors);
    tiles.loadTileset(tilesConfig("texture"), tilesConfig("tileWidth").toInt(), tilesConfig("tileHeight").toInt());

    // Load level 1
    level.load(1);

    // Setup the views
    auto defaultView = objects.window.getDefaultView();
    camera.setView("game", defaultView);
    camera.setView("background", defaultView, 0.5f);
    //camera.setView("menu", defaultView);

    // Load the background image
    bgTexture.loadFromFile("data/images/background.png");
    bgSprite.setTexture(bgTexture);
    bgSprite.setOrigin(bgTexture.getSize().x / 4, bgTexture.getSize().y / 4);
    // (Half for image to be centered, and half for the scaling)

    // Add player entity
    entities.emplace_back(new Player());
}

GameState::~GameState()
{
}

void GameState::onStart()
{
    //objects.music.start("game");
}

void GameState::handleEvents()
{
    sf::Event event;
    while (objects.window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                stateEvent.exitGame();
                break;

            case sf::Event::LostFocus:
                hasFocus = false;
                break;

            case sf::Event::GainedFocus:
                hasFocus = true;
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Space)
                    ((Player*)(entities.front().get()))->jump();
                break;

            default:
                break;
        }
    }
    //handleInput();
}

void GameState::update()
{
    physics.update(dt);

    // Set view's center based on player's position
    auto playerPos = entities.front()->position;
    auto playerSize = entities.front()->size;
    playerPos.x += playerSize.x / 2;
    playerPos.y += playerSize.y / 2;
    sf::Vector2f viewCenter(playerPos.x, playerPos.y);
    sf::Vector2f viewSize(camera.getView("game").getSize());

    // Make sure view isn't off the map
    if (viewCenter.x - (viewSize.x / 2) < 0)
        viewCenter.x = viewSize.x / 2;
    if (viewCenter.y - (viewSize.y / 2) < 0)
        viewCenter.y = viewSize.y / 2;
    if (viewCenter.x >= tiles.getPixelSize().x - (viewSize.x / 2))
        viewCenter.x = tiles.getPixelSize().x - (viewSize.x / 2);
    if (viewCenter.y >= tiles.getPixelSize().y - (viewSize.y / 2))
        viewCenter.y = tiles.getPixelSize().y - (viewSize.y / 2);

    camera.setCenter(viewCenter);
}

void GameState::draw()
{
    objects.window.clear();
    objects.window.setView(camera.getView("background"));
    objects.window.draw(bgSprite);
    objects.window.setView(camera.getView("game"));
    objects.window.draw(tiles);
    for (auto& ent: entities)
        objects.window.draw(*ent);
    objects.window.display();
}
