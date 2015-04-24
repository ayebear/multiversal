// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameworld.h"
#include "components.h"
#include "componentstrings.h"
#include "es/entityprototypeloader.h"
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

GameWorld::GameWorld(sf::RenderWindow& window, GameSaveHandler& gameSave):
    tileMapChanger(tileMapData, tileMap),
    level(tileMapData, tileMap, tileMapChanger, objects, magicWindow),
    levelLoader(level, gameSave, "data/levels/"),
    magicWindow(actions)
{
    std::cout << "Initializing GameWorld...\n";

    // Load actions
    actions.loadFromConfig("data/config/controls.cfg");

    // Setup systems
    systems.add<InputSystem>(window);
    systems.add<MovingSystem>(objects);
    systems.add<PlayerSystem>(objects, actions, level);
    systems.add<PhysicsSystem>(objects, tileMapData, tileMap, magicWindow, level);
    systems.add<CarrySystem>(objects, magicWindow);
    systems.add<SpriteSystem>(objects);
    systems.add<CameraSystem>(camera, tileMap);
    systems.add<TileSystem>(objects, tileMapData);
    systems.add<SwitchSystem>(tileMapData, tileMapChanger, objects);
    systems.add<ObjectSwitchSystem>(level, objects);
    systems.add<TileGroupSystem>(tileMapChanger, objects);
    systems.add<LaserSystem>(objects, tileMapData, tileMap, magicWindow);
    systems.add<RenderSystem>(objects, tileMap, window, camera, magicWindow);

    // Load entity prototypes
    bindComponentStrings(objects);
    if (!es::EntityPrototypeLoader::load(objects, "data/config/objects.cfg"))
        std::cerr << "ERROR: Could not load object prototypes.\n";

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

    std::cout << "GameWorld is now initialized.\n";
}
