// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamestate.h"
#include <iostream>
#include "gameobjects.h"
#include "events.h"
#include "broadcasts.h"
#include "components.h"
#include "gameevents.h"

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

    // TODO: Load the object prototypes from a file
    // For now, add them manually (still need to add deserialize methods)
    entities.addComponentsToPrototype("Player", Components::Position(), Components::Velocity(), Components::Size(), Components::AABB(),
    Components::Input(), Components::AnimSprite(), Components::PlayerState(), Components::CameraUpdater());

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
    auto playerId = entities.createObject("Player");
    auto animSprite = entities.getComponent<Components::AnimSprite>(playerId);
    if (animSprite)
    {
        animSprite->sprite.loadFromConfig("data/config/player.cfg");
        animSprite->sprite.play("StandRight");
        auto size = entities.getComponent<Components::Size>(playerId);
        if (size)
        {
            auto spriteSize = animSprite->sprite.getTileSize();
            size->x = spriteSize.x;
            size->y = spriteSize.y;
        }
    }

    // Setup the magic window
    static const unsigned magicWindowSize = 5;
    magicWindow.setSize(sf::Vector2f(tiles.getTileSize().x * magicWindowSize, tiles.getTileSize().y * magicWindowSize));
    auto magicWindowView = magicWindow.getTexture().getDefaultView();
    camera.setView("game2", magicWindowView);
    camera.setView("background2", magicWindowView, 0.5f);

    // Set the map size for the camera system
    cameraSystem.setMapSize(tiles.getPixelSize());
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
