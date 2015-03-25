// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamestate.h"
#include "gameresources.h"
#include "events.h"
#include "gameevents.h"
#include "game.h"

GameState::GameState(GameResources& resources):
    resources(resources),
    game(resources.window)
{
    auto& actions = game.getWorld().actions;
    actions("Game", "restartLevel").setCallback([]{ es::Events::send(ReloadLevelEvent{}); });
    actions("Game", "toggleMute").setCallback([&]{ resources.music.mute(); });
    actions("Game", "popState").setCallback([&]{ stateEvent.command = ng::StateEvent::Pop; });
}

void GameState::onStart()
{
    // Load a new level (or resumes from last save)
    game.initialize();

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
    game.update(dt);

    // Check if all levels have been completed
    if (es::Events::exists<GameFinishedEvent>())
    {
        stateEvent.command = ng::StateEvent::Change;
        stateEvent.name = "Final";
        es::Events::clear<GameFinishedEvent>();
    }

    resources.music.update();
}
