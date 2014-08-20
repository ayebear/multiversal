// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "playersystem.h"
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
        handleActionKey(playerState);
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
    auto carrier = entities.getComponent<Components::Carrier>(playerState.getOwnerID());
    if (velocity && sprite)
    {
        // TODO: Take in events from a an input system
        bool leftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A));
        bool rightPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D));
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

void PlayerSystem::handleActionKey(Components::PlayerState& playerState)
{
    // Handle the input for pressing "up", and proxy the events
    Broadcasts::clear<ActionKeyEvent>();
    auto position = entities.getComponent<Components::Position>(playerState.getOwnerID());
    if (position)
    {
        for (auto& event: Broadcasts::get<sf::Event>())
        {
            if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W))
            {
                // This is just for handling any tile actions (like going through exit)
                // TODO: Use actual tile size
                sf::Vector2i tilePos(position->x / 64, (position->y + 64) / 64);
                std::cout << "Action key pressed at location " << tilePos.x << ", " << tilePos.y << "\n";
                Broadcasts::send(ActionKeyEvent{tilePos, playerState.getOwnerID()});

                // This is for handling object actions (like picking up a box)
                // May need to use lists of object IDs that are colliding (or close by)
                // Update: May not need any extra info, just the event should be enough
            }
        }
    }
}
