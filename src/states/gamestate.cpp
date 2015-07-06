// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamestate.h"
#include "gameresources.h"
#include "es/events.h"
#include "gameevents.h"
#include "components.h"
#include "movingcomponent.h"
#include "lasercomponent.h"
#include "es/entityprototypeloader.h"
#include <iostream>

GameState::GameState(GameResources& resources):
    resources(resources),
    gameInstance(resources.window, resources.gameSave)
{
    // Register components and load entity prototypes
    es::registerComponents<Position, Velocity, Size, AABB, Sprite, AnimSprite, Jumpable, ObjectState, Movable, Carrier, Gravity, State, TileGroup, TilePosition, Rotation, Switch, InitialPosition, Prototype, CameraUpdater, AltWorld, DrawOnTop, Carryable, AboveWindow, Rigid, ExcludeFromLevel, Moving, Laser>();
    if (!es::loadPrototypes("data/config/entities.cfg"))
        std::cerr << "ERROR: Could not load object prototypes.\n";

    // Link action callbacks
    gameInstance.actions("Game", "restartLevel").setCallback([]{ es::Events::send(ReloadLevelEvent{}); });
    gameInstance.actions("Game", "toggleMute").setCallback([&]{ resources.music.mute(); });
    gameInstance.actions("Game", "popState").setCallback([&]{ stateEvent.command = ng::StateEvent::Pop; });
}

void GameState::onStart()
{
    // Load a new level, resumes from last save, or loads a test level
    if (!es::Events::exists<TestModeEvent>())
        gameInstance.levelLoader.clear();
    gameInstance.levelLoader.load();
    gameInstance.systems.initializeAll();

    // Start the game music
    resources.music.play("game");
}

void GameState::handleEvents()
{
    for (auto& event: es::Events::get<sf::Event>())
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = ng::StateEvent::Exit;
    }
}

void GameState::update()
{
    // Load the next level if needed
    if (gameInstance.levelLoader.update())
        gameInstance.systems.initializeAll();

    // Update the game view
    es::Events::send(ViewEvent{gameInstance.camera.getView("game")});

    es::Events::clear<ActionKeyEvent>();

    // Handle the events
    for (auto& event: es::Events::get<sf::Event>())
        gameInstance.actions.handleEvent(event);

    // Update all of the systems
    gameInstance.systems.updateAll(dt);

    // Update the magic window
    gameInstance.magicWindow.update();

    // Check if all levels have been completed
    if (es::Events::exists<GameFinishedEvent>())
    {
        stateEvent.command = ng::StateEvent::Change;
        stateEvent.name = "Final";
        es::Events::clear<GameFinishedEvent>();
    }

    resources.music.update();
}
