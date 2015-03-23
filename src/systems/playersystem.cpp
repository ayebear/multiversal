// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "playersystem.h"
#include "events.h"
#include "gameevents.h"
#include "level.h"
#include <iostream>

PlayerSystem::PlayerSystem(ocs::ObjectManager& entities, ng::ActionHandler& actions, Level& level):
    entities(entities),
    actions(actions),
    level(level),
    playerId(-1),
    wasRight(true)
{
    // Setup action callbacks
    actions("Player", "jump").setCallback(std::bind(&PlayerSystem::handleJump, this));
    actions("Player", "action").setCallback(std::bind(&PlayerSystem::handleAction, this));
}

void PlayerSystem::initialize()
{
    // Lookup the player's object ID
    playerId = level.getObjectIdFromName("player");
}

void PlayerSystem::update(float dt)
{
    handleMovement();
}

void PlayerSystem::handleMovement()
{
    auto velocity = entities.getComponent<Components::Velocity>(playerId);
    auto sprite = entities.getComponent<Components::AnimSprite>(playerId);
    auto movable = entities.getComponent<Components::Movable>(playerId);
    if (velocity && sprite && movable)
    {
        // Get status of actions
        bool leftPressed = actions("Player", "moveLeft").isActive();
        bool rightPressed = actions("Player", "moveRight").isActive();

        // Check the carrying state
        std::string carryStr;
        auto carrier = entities.getComponent<Components::Carrier>(playerId);
        if (carrier && carrier->carrying)
            carryStr = "Carry";

        // Change velocity and animation based on input
        if (leftPressed && !rightPressed)
        {
            velocity->x = -movable->velocity;
            sprite->sprite.play("MoveLeft" + carryStr);
            wasRight = false;

        }
        else if (rightPressed && !leftPressed)
        {
            velocity->x = movable->velocity;
            sprite->sprite.play("MoveRight" + carryStr);
            wasRight = true;
        }
        else
        {
            velocity->x = 0;
            if (wasRight)
                sprite->sprite.play("StandRight" + carryStr);
            else
                sprite->sprite.play("StandLeft" + carryStr);
        }
    }
}

void PlayerSystem::handleJump()
{
    auto jumpable = entities.getComponent<Components::Jumpable>(playerId);
    auto objectState = entities.getComponent<Components::ObjectState>(playerId);
    auto velocity = entities.getComponent<Components::Velocity>(playerId);
    if (jumpable && objectState && velocity)
    {
        // Jump by changing velocity if not already in the air
        if (objectState->state == Components::ObjectState::OnPlatform)
        {
            velocity->y = jumpable->jumpSpeed;
            objectState->state = Components::ObjectState::InAir;
        }
    }
}

void PlayerSystem::handleAction()
{
    // Handle the input for pressing "up", and proxy the events
    auto position = entities.getComponent<Components::Position>(playerId);
    auto aabb = entities.getComponent<Components::AABB>(playerId);
    if (position && aabb)
    {
        std::cout << "Action key was pressed. Locations: ";
        for (int location: aabb->tileCollisions)
            std::cout << location << ' ';
        std::cout << '\n';
        es::Events::send(ActionKeyEvent{playerId});
    }
}
