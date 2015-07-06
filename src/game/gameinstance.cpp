// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameinstance.h"
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

GameInstance::GameInstance(sf::RenderWindow& window, GameSaveHandler& gameSave):
    tileMapChanger(tileMapData, tileMap),
    level(tileMapData, tileMap, tileMapChanger, world, magicWindow),
    levelLoader(level, gameSave, "data/levels/"),
    magicWindow(actions)
{
    std::cout << "Initializing GameInstance...\n";

    // Load actions
    actions.loadFromConfig("data/config/controls.cfg");

    // Setup systems
    systems.add<InputSystem>(window);
    systems.add<MovingSystem>(world);
    systems.add<PlayerSystem>(world, actions, level);
    systems.add<PhysicsSystem>(world, tileMapData, tileMap, magicWindow, level);
    systems.add<CarrySystem>(world, magicWindow);
    systems.add<SpriteSystem>(world);
    systems.add<CameraSystem>(camera, tileMap);
    systems.add<TileSystem>(world, tileMapData);
    systems.add<SwitchSystem>(tileMapData, tileMapChanger, world);
    systems.add<ObjectSwitchSystem>(level, world);
    systems.add<TileGroupSystem>(tileMapChanger, world);
    systems.add<LaserSystem>(world, tileMapData, tileMap, magicWindow);
    systems.add<RenderSystem>(world, tileMap, window, camera, magicWindow);

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

    std::cout << "GameInstance is now initialized.\n";
}
