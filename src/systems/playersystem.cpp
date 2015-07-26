// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "playersystem.h"
#include "es/events.h"
#include "gameevents.h"
#include "level.h"

PlayerSystem::PlayerSystem(es::World& world, ng::ActionHandler& actions, Level& level):
    world(world),
    actions(actions),
    level(level),
    playerId(es::invalidId),
    wasRight(true)
{
    // Setup action callbacks
    actions("Player", "jump").setCallback(std::bind(&PlayerSystem::handleJump, this));
    actions("Player", "action").setCallback(std::bind(&PlayerSystem::handleAction, this));
}

void PlayerSystem::initialize()
{
    // Create a player object if it doesn't already exist
    playerId = world("Player", "player").getId();
}

void PlayerSystem::update(float dt)
{
    handleMovement();
}

void PlayerSystem::handleMovement()
{
    auto player = world[playerId];
    auto velocity = player.get<Velocity>();
    auto sprite = player.get<AnimSprite>();
    auto movable = player.get<Movable>();
    if (velocity && sprite && movable)
    {
        // Get status of actions
        bool leftPressed = actions("Player", "moveLeft").isActive();
        bool rightPressed = actions("Player", "moveRight").isActive();

        // Check the carrying state
        std::string carryStr;
        auto carrier = player.get<Carrier>();
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
    auto player = world[playerId];
    auto jumpable = player.get<Jumpable>();
    auto objectState = player.get<ObjectState>();
    auto velocity = player.get<Velocity>();
    if (jumpable && objectState && velocity)
    {
        // Jump by changing velocity if not already in the air
        if (objectState->state == ObjectState::OnPlatform)
        {
            velocity->y = jumpable->jumpSpeed;
            objectState->state = ObjectState::InAir;
        }
    }
}

void PlayerSystem::handleAction()
{
    // Handle the input for pressing the action key, and proxy the events
    auto player = world[playerId];
    auto position = player.get<Position>();
    auto aabb = player.get<AABB>();
    if (position && aabb)
        es::Events::send(ActionKeyEvent{playerId});
}
