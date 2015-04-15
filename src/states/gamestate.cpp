// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamestate.h"
#include "gameresources.h"
#include "es/events.h"
#include "gameevents.h"

GameState::GameState(GameResources& resources):
    resources(resources),
    world(resources.window)
{
    world.actions("Game", "restartLevel").setCallback([]{ es::Events::send(ReloadLevelEvent{}); });
    world.actions("Game", "toggleMute").setCallback([&]{ resources.music.mute(); });
    world.actions("Game", "popState").setCallback([&]{ stateEvent.command = ng::StateEvent::Pop; });
}

void GameState::onStart()
{
    // Load a new level, resumes from last save, or loads a test level
    if (!es::Events::exists<TestModeEvent>())
        world.levelLoader.clear();
    world.levelLoader.load();
    world.systems.initializeAll();

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

    // Check if all levels have been completed
    if (es::Events::exists<GameFinishedEvent>())
    {
        stateEvent.command = ng::StateEvent::Change;
        stateEvent.name = "Final";
        es::Events::clear<GameFinishedEvent>();
    }

    resources.music.update();
}
