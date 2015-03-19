// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamestate.h"
#include "gameobjects.h"
#include "events.h"
#include "gameevents.h"
#include "game.h"

GameState::GameState(GameObjects& objects, Game& game):
    objects(objects),
    game(game)
{
    auto& actions = game.getWorld().actions;
    actions("Game", "restartLevel").setCallback([]{ es::Events::send(ReloadLevelEvent{}); });
    actions("Game", "toggleMute").setCallback([&]{ objects.music.mute(); });
    actions("Game", "popState").setCallback([&]{ stateEvent.command = StateEvent::Pop; });
}

void GameState::onStart()
{
    // Load a new level (or resumes from last save)
    game.initialize();

    // Start the game music
    objects.music.play("game");
}

void GameState::handleEvents()
{
    for (auto& event: es::Events::get<sf::Event>())
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = StateEvent::Exit;
    }
}

void GameState::update()
{
    game.update(dt);

    // Check if all levels have been completed
    if (es::Events::exists<GameFinishedEvent>())
    {
        stateEvent.command = StateEvent::Change;
        stateEvent.name = "Final";
        es::Events::clear<GameFinishedEvent>();
    }

    objects.music.update();
}
