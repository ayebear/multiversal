// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamestate.h"
#include <iostream>
#include "gameobjects.h"
#include "events.h"
#include "components.h"
#include "gameevents.h"
#include "entityprototypeloader.h"
#include "componentstrings.h"
#include "windowfocus.h"

GameState::GameState(GameObjects& objects):
    objects(objects),
    level("data/levels/", tileMapData, tileMap, entities),
    inputSystem(objects.window),
    playerSystem(entities),
    physicsSystem(entities, tileMapData, tileMap, magicWindow),
    carrySystem(entities, magicWindow),
    spritePositionSystem(entities),
    cameraSystem(camera),
    renderSystem(entities, tileMap, objects.window, camera, magicWindow)
{
    // Load entity prototypes
    bindComponentStrings(entities);
    EntityPrototypeLoader::load(entities, "data/config/entities.cfg");

    // Load the tiles
    cfg::File tilesConfig("data/config/tiles.cfg", cfg::File::Warnings | cfg::File::Errors);
    tileMap.loadTileset(tilesConfig("texture"), tilesConfig("tileWidth").toInt(), tilesConfig("tileHeight").toInt());

    // Load level 1
    level.load(1);

    // Setup the views
    auto defaultView = objects.window.getDefaultView();
    camera.setView("game", defaultView);
    camera.setView("background", defaultView, 0.5f);
    //camera.setView("menu", defaultView);

    // Setup the magic window
    static const unsigned magicWindowSize = 5;
    magicWindow.setSize(sf::Vector2f(tileMap.getTileSize().x * magicWindowSize, tileMap.getTileSize().y * magicWindowSize));
    auto magicWindowView = magicWindow.getTexture().getDefaultView();
    camera.setView("game2", magicWindowView);
    camera.setView("background2", magicWindowView, 0.5f);

    // Set the map size for the camera system
    cameraSystem.setMapSize(tileMap.getPixelSize());
}

void GameState::handleEvents()
{
    inputSystem.update(camera.getView("game"));
    for (auto& event: Events::get<sf::Event>())
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                stateEvent.command = StateEvent::Exit;
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    stateEvent.command = StateEvent::Exit;
                break;

            case sf::Event::LostFocus:
                WindowFocus::setFocus(false);
                break;

            case sf::Event::GainedFocus:
                WindowFocus::setFocus(true);
                break;

            default:
                break;
        }
    }
}

void GameState::update()
{
    magicWindow.update();
    playerSystem.update(dt);
    physicsSystem.update(dt);
    carrySystem.update();
    spritePositionSystem.update(dt);
    cameraSystem.update();
}

void GameState::draw()
{
    renderSystem.update();
}
