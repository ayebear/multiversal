// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameworld.h"
#include "components.h"
#include "componentstrings.h"
#include "entityprototypeloader.h"
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

GameWorld::GameWorld(sf::RenderWindow& window):
    tileMapChanger(tileMapData, tileMap),
    level(tileMapData, tileMap, tileMapChanger, entities, magicWindow),
    levelLoader(level, "data/levels/"),
    magicWindow(actions)
{
    std::cout << "Initializing GameWorld...\n";

    // Load actions
    actions.loadFromConfig("data/config/controls.cfg");

    // Setup systems
    systems.add<InputSystem>(window);
    systems.add<MovingSystem>(entities);
    systems.add<PhysicsSystem>(entities, tileMapData, tileMap, magicWindow);
    systems.add<PlayerSystem>(entities, actions, level);
    systems.add<CarrySystem>(entities, magicWindow);
    systems.add<SpriteSystem>(entities);
    systems.add<CameraSystem>(camera, tileMap);
    systems.add<TileSystem>(entities, tileMapData);
    systems.add<SwitchSystem>(tileMapData, tileMapChanger, entities);
    systems.add<ObjectSwitchSystem>(level, entities);
    systems.add<TileGroupSystem>(tileMapChanger, entities);
    systems.add<LaserSystem>(entities, tileMapData, tileMap, magicWindow);
    systems.add<RenderSystem>(entities, tileMap, window, camera, magicWindow);

    // Load entity prototypes
    bindComponentStrings(entities);
    es::EntityPrototypeLoader::load(entities, "data/config/entities.cfg");

    // Load the tiles
    tileMap.loadFromConfig("data/config/tilemap.cfg");

    // Setup the views
    auto defaultView = window.getDefaultView();
    camera.setView("window", defaultView);

    camera.setView("game", defaultView);
    camera.setView("background", defaultView, 0.5f);

    // Setup the magic window
    magicWindow.setTileSize(tileMap.getTileSize());
    auto magicWindowView = magicWindow.getRenderTexture().getDefaultView();
    camera.setView("game2", magicWindowView);
    camera.setView("background2", magicWindowView, 0.5f);

    std::cout << "GameWorld is now loaded.\n";
}
