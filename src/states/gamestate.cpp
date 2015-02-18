// Copyright (C) 2014-2015 Eric Hebert (ayebear)
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
#include "movingsystem.h"
#include "physicssystem.h"
#include "carrysystem.h"
#include "spritepositionsystem.h"
#include "camerasystem.h"
#include "tilesystem.h"
#include "switchsystem.h"
#include "forcefieldsystem.h"
#include "objectswitchsystem.h"
#include "rendersystem.h"

GameState::GameState(GameObjects& objects):
    objects(objects),
    tileMapChanger(tileMapData, tileMap),
    level("data/levels/", tileMapData, tileMap, tileMapChanger, entities, magicWindow)
{
    // Setup systems
    systems.add<InputSystem>(objects.window);
    systems.add<MovingSystem>(entities);
    systems.add<PhysicsSystem>(entities, tileMapData, tileMap, magicWindow);
    systems.add<PlayerSystem>(entities, tileMap);
    systems.add<CarrySystem>(entities, magicWindow);
    systems.add<SpritePositionSystem>(entities);
    systems.add<CameraSystem>(camera);
    systems.add<TileSystem>(entities, tileMapData);
    systems.add<SwitchSystem>(tileMapData, tileMapChanger);
    systems.add<ForceFieldSystem>(tileMapChanger);
    systems.add<ObjectSwitchSystem>(level, entities);
    systems.add<RenderSystem>(entities, tileMap, objects.window, camera, magicWindow);

    // Load entity prototypes
    bindComponentStrings(entities);
    EntityPrototypeLoader::load(entities, "data/config/entities.cfg");

    // Load the tiles
    cfg::File tilesConfig("data/config/tilemap.cfg", cfg::File::Warnings | cfg::File::Errors);
    tileMap.loadTileset(tilesConfig("texture"), tilesConfig("tileWidth").toInt(), tilesConfig("tileHeight").toInt());

    // Setup the views
    auto defaultView = objects.window.getDefaultView();
    camera.setView("game", defaultView);
    camera.setView("background", defaultView, 0.5f);
    //camera.setView("menu", defaultView);

    // Setup the magic window
    magicWindow.setTileSize(tileMap.getTileSize());
    auto magicWindowView = magicWindow.getRenderTexture().getDefaultView();
    camera.setView("game2", magicWindowView);
    camera.setView("background2", magicWindowView, 0.5f);
}

void GameState::onStart()
{
    // Clear any old events from the last time it was ran
    es::Events::clearAll();

    // Reset the window size
    magicWindow.setSize();

    // Load level 1 and initialize the systems
    level.load(1);
    systems.initialize();

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
                    stateEvent.command = StateEvent::Pop;
                else if (event.key.code == sf::Keyboard::R)
                {
                    // Reload the current level
                    level.load();
                    systems.initialize();
                }
                else if (event.key.code == sf::Keyboard::M)
                    objects.music.mute(); // Mute the music
                break;

            // TODO: Get rid of these after updating to SFML 2.2
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

    // Load the next level if needed
    if (level.update())
        systems.initialize();

    // Check if all levels have been completed
    if (es::Events::exists<GameFinishedEvent>())
    {
        stateEvent.command = StateEvent::Change;
        stateEvent.name = "Final";
        es::Events::clear<GameFinishedEvent>();
    }

    // Update all of the systems
    systems.update(dt);

    magicWindow.update();
    objects.music.update();
}
