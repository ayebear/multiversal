// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "game.h"
#include "events.h"
#include "components.h"
#include "gameevents.h"
#include "entityprototypeloader.h"
#include "componentstrings.h"
#include <iostream>

// Include systems
#include "inputsystem.h"
#include "playersystem.h"
#include "movingsystem.h"
#include "physicssystem.h"
#include "carrysystem.h"
#include "spritesystem.h"
#include "camerasystem.h"
#include "tilesystem.h"
#include "switchsystem.h"
#include "objectswitchsystem.h"
#include "tilegroupsystem.h"
#include "lasersystem.h"
#include "rendersystem.h"

Game::Game(sf::RenderWindow& window):
    window(window)
{
    // Setup systems
    world.systems.add<InputSystem>(window);
    world.systems.add<MovingSystem>(world.entities);
    world.systems.add<PhysicsSystem>(world.entities, world.tileMapData, world.tileMap, world.magicWindow);
    world.systems.add<PlayerSystem>(world.entities, world.actions, world.level);
    world.systems.add<CarrySystem>(world.entities, world.magicWindow);
    world.systems.add<SpriteSystem>(world.entities);
    world.systems.add<CameraSystem>(world.camera, world.tileMap);
    world.systems.add<TileSystem>(world.entities, world.tileMapData);
    world.systems.add<SwitchSystem>(world.tileMapData, world.tileMapChanger);
    world.systems.add<ObjectSwitchSystem>(world.level, world.entities);
    world.systems.add<TileGroupSystem>(world.tileMapChanger, world.entities);
    world.systems.add<LaserSystem>(world.entities, world.tileMapData, world.tileMap, world.magicWindow);
    world.systems.add<RenderSystem>(world.entities, world.tileMap, window, world.camera, world.magicWindow);

    // Load entity prototypes
    bindComponentStrings(world.entities);
    es::EntityPrototypeLoader::load(world.entities, "data/config/entities.cfg");

    // Load the tiles
    cfg::File tilesConfig("data/config/tilemap.cfg", cfg::File::Warnings | cfg::File::Errors);
    world.tileMap.loadTileset(tilesConfig("texture"), tilesConfig("tileWidth").toInt(), tilesConfig("tileHeight").toInt());

    // Setup the views
    auto defaultView = window.getDefaultView();

    world.camera.setView("game", defaultView);
    world.camera.setView("background", defaultView, 0.5f);

    // Setup the magic window
    world.magicWindow.setTileSize(world.tileMap.getTileSize());
    auto magicWindowView = world.magicWindow.getRenderTexture().getDefaultView();
    world.camera.setView("game2", magicWindowView);
    world.camera.setView("background2", magicWindowView, 0.5f);
}

void Game::initialize()
{
    world.levelLoader.load();
    world.systems.initializeAll();
}

void Game::update(float dt)
{
    // Load the next level if needed
    if (world.levelLoader.update())
        world.systems.initializeAll();

    // Update the game view
    es::Events::send(ViewEvent{world.camera.getView("game")});

    es::Events::clear<ActionKeyEvent>();

    // Handle the events
    for (auto& event: es::Events::get<sf::Event>())
        world.actions.handleEvent(event);

    // Update all of the systems
    world.systems.updateAll(dt);

    // Update the magic window
    world.magicWindow.update();
}

GameWorld& Game::getWorld()
{
    return world;
}
