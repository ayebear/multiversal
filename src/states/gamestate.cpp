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
        switch (event.type)
        {
            case sf::Event::Closed:
                stateEvent.command = StateEvent::Exit;
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    stateEvent.command = StateEvent::Pop; // Go back to the menu
                else if (event.key.code == sf::Keyboard::R)
                    es::Events::send(ReloadLevelEvent()); // Reload the level
                else if (event.key.code == sf::Keyboard::M)
                    objects.music.mute(); // Mute the music
                break;

            default:
                break;
        }
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
