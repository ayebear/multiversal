// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamestate.h"
#include <iostream>
#include "gameobjects.h"
#include "events.h"
#include "broadcasts.h"
#include "components.h"
#include "gameevents.h"
#include "entityprototypeloader.h"

GameState::GameState(GameObjects& objects):
    objects(objects),
    level("data/levels/", tiles),
    input(objects.window),
    player(entities),
    physics(entities, tiles, magicWindow),
    cameraSystem(camera),
    render(entities, tiles, objects.window, camera, magicWindow)
{
    hasFocus = true;

    // Load entity prototypes
    bindStringsToComponents();
    EntityPrototypeLoader::load(entities, "data/config/entities.cfg");

    // Load the tileset
    cfg::File tilesConfig("data/config/tiles.cfg", cfg::File::Warnings | cfg::File::Errors);
    tiles.loadTileset(tilesConfig("texture"), tilesConfig("tileWidth").toInt(), tilesConfig("tileHeight").toInt());

    // Load level 1
    level.load(1);

    // Setup the views
    auto defaultView = objects.window.getDefaultView();
    camera.setView("game", defaultView);
    camera.setView("background", defaultView, 0.5f);
    //camera.setView("menu", defaultView);

    // Add player entity
    entities.createObject("Player");

    // Setup the magic window
    static const unsigned magicWindowSize = 5;
    magicWindow.setSize(sf::Vector2f(tiles.getTileSize().x * magicWindowSize, tiles.getTileSize().y * magicWindowSize));
    auto magicWindowView = magicWindow.getTexture().getDefaultView();
    camera.setView("game2", magicWindowView);
    camera.setView("background2", magicWindowView, 0.5f);

    // Set the map size for the camera system
    cameraSystem.setMapSize(tiles.getPixelSize());
}

void GameState::bindStringsToComponents()
{
    entities.bindStringToComponent<Components::Position>("Position");
    entities.bindStringToComponent<Components::Velocity>("Velocity");
    entities.bindStringToComponent<Components::Size>("Size");
    entities.bindStringToComponent<Components::AABB>("AABB");
    entities.bindStringToComponent<Components::Input>("Input");
    entities.bindStringToComponent<Components::AnimSprite>("AnimSprite");
    entities.bindStringToComponent<Components::PlayerState>("PlayerState");
    entities.bindStringToComponent<Components::CameraUpdater>("CameraUpdater");
}

void GameState::handleEvents()
{
    input.update(camera.getView("game"));
    for (auto& event: Broadcasts::get<sf::Event>())
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                stateEvent.exitGame();
                break;

            case sf::Event::LostFocus:
                hasFocus = false;
                break;

            case sf::Event::GainedFocus:
                hasFocus = true;
                break;

            default:
                break;
        }
    }
}

void GameState::update()
{
    magicWindow.update();
    player.update(dt);
    physics.update(dt);
    cameraSystem.update();
}

void GameState::draw()
{
    render.update();
}
