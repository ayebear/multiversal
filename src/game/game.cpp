// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "game.h"
#include "events.h"
#include "gameevents.h"

Game::Game(sf::RenderWindow& window):
    window(window),
    world(window)
{
}

void Game::initialize()
{
    if (!es::Events::exists<TestModeEvent>())
        world.levelLoader.clear();
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
