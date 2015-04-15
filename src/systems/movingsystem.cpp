// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "movingsystem.h"
#include "es/events.h"
#include "gameevents.h"
#include "nage/graphics/vectors.h"
#include <iostream>

MovingSystem::MovingSystem(ocs::ObjectManager& objects):
    objects(objects)
{
}

void MovingSystem::initialize()
{
    // Move all objects with Moving components to the first point
    for (auto& moving: objects.getComponentArray<Components::Moving>())
    {
        auto position = objects.getComponent<Components::Position>(moving.getOwnerID());
        if (position && !moving.points.empty())
        {
            auto point = moving.points.front();
            position->x = point.x;
            position->y = point.y;
        }
    }
}

void MovingSystem::update(float dt)
{
    for (auto& moving: objects.getComponentArray<Components::Moving>())
    {
        auto position = objects.getComponent<Components::Position>(moving.getOwnerID());
        auto state = objects.getComponent<Components::State>(moving.getOwnerID());
        if (position && state)
        {
            // Make sure nothing goes out of bounds
            if (moving.currentPoint >= static_cast<int>(moving.points.size()))
                moving.currentPoint = 0;

            // Start moving if the state changes
            if (state->hasChanged())
            {
                moving.isMoving = true;
                goToNextPoint(moving, *position, *state);
            }
            else if (moving.isMoving)
            {
                // Update the position from our velocity vector
                position->x += moving.velocity.x * dt;
                position->y += moving.velocity.y * dt;

                sf::Vector2f currentPos(position->x, position->y);

                // Check if the object has moved far enough for the next point
                if (ng::vectors::distance(moving.startPos, currentPos) >= moving.distance)
                {
                    // Lock in the position
                    auto& destination = moving.points[moving.currentPoint];
                    position->x = destination.x;
                    position->y = destination.y;

                    goToNextPoint(moving, *position, *state);
                }
            }
        }
    }
}

void MovingSystem::goToNextPoint(Components::Moving& moving, Components::Position& position, Components::State& state) const
{
    // Go to the next point (depends on state as well)
    moving.currentPoint += (state.value ? 1 : -1);
    int last = moving.points.size() - 1;

    // Loop and wraparound, or just stay at the same point and stop
    if (moving.currentPoint > last)
    {
        moving.currentPoint = (moving.loop ? 0 : last);
        if (!moving.loop)
            moving.isMoving = false;
    }
    else if (moving.currentPoint < 0)
    {
        moving.currentPoint = (moving.loop ? last : 0);
        if (!moving.loop)
            moving.isMoving = false;
    }

    // Calculate the new velocity vector and distance
    calculateVelocity(moving, position);
}

void MovingSystem::calculateVelocity(Components::Moving& moving, Components::Position& position) const
{
    auto end = moving.points[moving.currentPoint];
    sf::Vector2f start(position.x, position.y);

    // Calculate the difference between the starting and ending points
    sf::Vector2f diff = end - start;

    // Normalize vector (make unit vector) by dividing length
    ng::vectors::normalize(diff);

    // Multiply unit vector by speed to get velocity
    moving.velocity = diff * moving.speed;

    // Calculate the distance to the end point
    moving.distance = ng::vectors::distance(start, end);

    // Save the starting position
    moving.startPos = sf::Vector2f(position.x, position.y);
}
