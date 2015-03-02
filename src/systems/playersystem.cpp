// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "playersystem.h"
#include "events.h"
#include "gameevents.h"
#include "tilemap.h"
#include <iostream>

PlayerSystem::PlayerSystem(ocs::ObjectManager& entities, TileMap& tileMap, sf::RenderWindow& window):
    entities(entities),
    tileMap(tileMap),
    window(window)
{
}

void PlayerSystem::update(float dt)
{
    for (auto& playerState: entities.getComponentArray<Components::PlayerState>())
    {
        handleJumps(playerState);
        handleMovement(playerState);
        handlePosition(playerState);
        handleActionKey(playerState);
    }
    es::Events::clear<PlayerPosition>();
}

void PlayerSystem::handleJumps(Components::PlayerState& playerState)
{
    for (auto& event: es::Events::get<OnPlatformEvent>())
    {
        if (event.entityId == playerState.getOwnerID())
        {
            if (event.state)
                playerState.state = Components::PlayerState::OnPlatform;
            else
                playerState.state = Components::PlayerState::InAir;
        }
    }
    for (auto& event: es::Events::get<sf::Event>())
    {
        if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Space ||
            event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up))
        {
            auto velocity = entities.getComponent<Components::Velocity>(playerState.getOwnerID());
            if (velocity)
            {
                if (true || playerState.state == Components::PlayerState::OnPlatform)
                {
                    velocity->y = Components::PlayerState::jumpSpeed;
                    playerState.state = Components::PlayerState::InAir;
                }
            }
        }
    }
}

void PlayerSystem::handleMovement(Components::PlayerState& playerState)
{
    auto velocity = entities.getComponent<Components::Velocity>(playerState.getOwnerID());
    auto sprite = entities.getComponent<Components::AnimSprite>(playerState.getOwnerID());
    auto carrier = entities.getComponent<Components::Carrier>(playerState.getOwnerID());
    if (velocity && sprite)
    {
        bool hasFocus = window.hasFocus();

        // TODO: Come up with a workaround
        // SFML 2.2 window focus seems to be broken in fullscreen
        hasFocus = true;

        // TODO: Take in events from an input system
        bool leftPressed = (hasFocus && (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)));
        bool rightPressed = (hasFocus && (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)));
        std::string carryStr;
        if (carrier && carrier->carrying)
            carryStr = "Carry";
        if (leftPressed && !rightPressed)
        {
            velocity->x = -1600;
            sprite->sprite.play("MoveLeft" + carryStr);
            playerState.wasRight = false;

        }
        else if (rightPressed && !leftPressed)
        {
            velocity->x = 1600;
            sprite->sprite.play("MoveRight" + carryStr);
            playerState.wasRight = true;
        }
        else
        {
            velocity->x = 0;
            if (playerState.wasRight)
                sprite->sprite.play("StandRight" + carryStr);
            else
                sprite->sprite.play("StandLeft" + carryStr);
        }
    }
}

void PlayerSystem::handlePosition(Components::PlayerState& playerState)
{
    auto position = entities.getComponent<Components::Position>(playerState.getOwnerID());
    if (position)
    {
        // This could just be an entity position update, and just have the player entity ID in the event
        auto& events = es::Events::get<PlayerPosition>();
        for (auto& event: events)
        {
            position->x = event.position.x;
            position->y = event.position.y;
            std::cout << "Received PlayerPosition event: " << event.position.x << ", " << event.position.y << "\n";
        }
    }
}

void PlayerSystem::handleActionKey(Components::PlayerState& playerState)
{
    // Handle the input for pressing "up", and proxy the events
    es::Events::clear<ActionKeyEvent>();
    auto position = entities.getComponent<Components::Position>(playerState.getOwnerID());
    auto aabb = entities.getComponent<Components::AABB>(playerState.getOwnerID());
    if (position)
    {
        for (auto& event: es::Events::get<sf::Event>())
        {
            if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S))
            {
                std::cout << "Action key was pressed. Locations: ";
                for (int location: aabb->tileCollisions)
                    std::cout << location << ' ';
                std::cout << '\n';
                es::Events::send(ActionKeyEvent{playerState.getOwnerID()});
            }
        }
    }
}
