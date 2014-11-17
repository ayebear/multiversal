// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "playersystem.h"
#include "events.h"
#include "events.h"
#include "gameevents.h"
#include "windowfocus.h"
#include "tilemap.h"
#include <iostream>

PlayerSystem::PlayerSystem(ocs::ObjectManager& entities, TileMap& tileMap):
    entities(entities),
    tileMap(tileMap)
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
            //std::cout << "Player state: " << event.state << "\n";
            if (event.state)
                playerState.state = Components::PlayerState::OnPlatform;
            else
                playerState.state = Components::PlayerState::InAir;
        }
    }
    for (auto& event: es::Events::get<sf::Event>())
    {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
        {
            auto velocity = entities.getComponent<Components::Velocity>(playerState.getOwnerID());
            if (velocity)
            {
                if (playerState.state == Components::PlayerState::OnPlatform)
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
        // TODO: Take in events from a an input system
        bool leftPressed = (WindowFocus::hasFocus() && (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)));
        bool rightPressed = (WindowFocus::hasFocus() && (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)));
        std::string carryStr;
        if (carrier && carrier->carrying)
            carryStr = "Carry";
        if (leftPressed && !rightPressed)
        {
            velocity->x = -400;
            sprite->sprite.play("MoveLeft" + carryStr);
            playerState.wasRight = false;

        }
        else if (rightPressed && !leftPressed)
        {
            velocity->x = 400;
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
        //std::cout << "Position: " << position->x << ", " << position->y << "\n";
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
            if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W))
            {
                //auto tileSize = tileMap.getTileSize();
                //sf::Vector2i tilePos(position->x / tileSize.x, (position->y + tileSize.y) / tileSize.y);
                // Note: All of the colliding tiles are stored in the tileCollisions vector in the AABB component
                //std::cout << "Action key pressed at location " << tilePos.x << ", " << tilePos.y << "\n";
                std::cout << "Action key was pressed. Locations: ";
                for (auto& location: aabb->tileCollisions)
                    std::cout << location.x << ", " << location.y << "\n";
                std::cout << "\n";
                es::Events::send(ActionKeyEvent{playerState.getOwnerID()});
            }
        }
    }
}
