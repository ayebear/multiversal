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

// Include systems
#include "inputsystem.h"
#include "playersystem.h"
#include "physicssystem.h"
#include "carrysystem.h"
#include "spritepositionsystem.h"
#include "camerasystem.h"
#include "tilesystem.h"
#include "rendersystem.h"

GameState::GameState(GameObjects& objects):
    objects(objects),
    level("data/levels/", tileMapData, tileMap, entities)
{
    // Setup systems
    systems.add<InputSystem>(objects.window);
    systems.add<PhysicsSystem>(entities, tileMapData, tileMap, magicWindow);
    systems.add<PlayerSystem>(entities, tileMap);
    systems.add<CarrySystem>(entities, magicWindow);
    systems.add<SpritePositionSystem>(entities);
    systems.add<CameraSystem>(camera);
    systems.add<TileSystem>(entities, tileMapData);
    systems.add<RenderSystem>(entities, tileMap, objects.window, camera, magicWindow);

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

    // Start the game music
    objects.music.play("game");
}

void GameState::handleEvents()
{
    // TODO: Will need to update the input system here, or these events will be a frame behind
    for (auto& event: es::Events::get<sf::Event>())
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                stateEvent.command = StateEvent::Exit;
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    stateEvent.command = StateEvent::Exit;
                else if (event.key.code == sf::Keyboard::R)
                    level.load(); // Reload the current level
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
    // The input system needs the game view
    es::Events::clear<GameViewEvent>();
    es::Events::send(GameViewEvent{camera.getView("game")});

    level.update();

    // Update all of the systems
    systems.update(dt);

    magicWindow.update();
    objects.music.update();
}
