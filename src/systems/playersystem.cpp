// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "playersystem.h"
#include "OCS/Objects.hpp"
#include "events.h"
#include "broadcasts.h"
#include "gameevents.h"

PlayerSystem::PlayerSystem(ocs::ObjectManager& entities):
    entities(entities)
{
}

void PlayerSystem::update(float dt)
{
    for (auto& playerState: entities.getComponentArray<Components::PlayerState>())
    {
        handleJumps(playerState);
        handleMovement(playerState);
        handlePosition(playerState);
    }
    Events::clear<sf::Vector2f>("PlayerPosition");
}

void PlayerSystem::handleJumps(Components::PlayerState& playerState)
{
    for (auto& event: Broadcasts::get<OnPlatformEvent>())
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
    for (auto& event: Broadcasts::get<sf::Event>())
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
    if (velocity && sprite)
    {
        // TODO: Take in events from a an input system
        bool leftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A));
        bool rightPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D));
        if (leftPressed && !rightPressed)
        {
            velocity->x = -400;
            sprite->sprite.play("MoveLeft");
            playerState.wasRight = false;

        }
        else if (rightPressed && !leftPressed)
        {
            velocity->x = 400;
            sprite->sprite.play("MoveRight");
            playerState.wasRight = true;
        }
        else
        {
            velocity->x = 0;
            if (playerState.wasRight)
                sprite->sprite.play("StandRight");
            else
                sprite->sprite.play("StandLeft");
        }
    }
}

void PlayerSystem::handlePosition(Components::PlayerState& playerState)
{
    auto position = entities.getComponent<Components::Position>(playerState.getOwnerID());
    if (position)
    {
        // This could just be an entity position update, and just have the player entity ID in the event
        auto& events = Events::get<sf::Vector2f>("PlayerPosition");
        for (auto& event: events)
        {
            position->x = event.x;
            position->y = event.y;
            std::cout << "Received PlayerPosition event: " << event.x << ", " << event.y << "\n";
        }
        //std::cout << "Position: " << position->x << ", " << position->y << "\n";
    }
}
