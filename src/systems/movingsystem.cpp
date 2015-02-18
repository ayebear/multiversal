#include "movingsystem.h"
#include "events.h"
#include "gameevents.h"
#include "vectors.h"
#include <iostream>

MovingSystem::MovingSystem(ocs::ObjectManager& entities):
    entities(entities)
{
}

void MovingSystem::initialize()
{
    std::cout << "MovingSystem::initialize()\n";
    // Move all objects with Moving components to the first point
    for (auto& moving: entities.getComponentArray<Components::Moving>())
    {
        auto position = entities.getComponent<Components::Position>(moving.getOwnerID());
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
    processEvents();
    for (auto& moving: entities.getComponentArray<Components::Moving>())
    {
        auto position = entities.getComponent<Components::Position>(moving.getOwnerID());
        auto state = entities.getComponent<Components::State>(moving.getOwnerID());
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
                if (vectors::distance(moving.startPos, currentPos) >= moving.distance)
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

void MovingSystem::processEvents()
{
    // TODO: Determine if this is needed
    // Handle events that can change the state of moving components
    // Note: May need to map a "moving component ID" to a regular object ID
    for (auto& event: es::Events::get<MovingEvent>())
    {
        std::cout << "~~~~~ Received MovingEvent ~~~~~\n";
        auto moving = entities.getComponent<Components::Moving>(event.entityId);
        auto position = entities.getComponent<Components::Position>(event.entityId);
        auto state = entities.getComponent<Components::State>(event.entityId);
        if (moving && position && state && state->value != event.state)
        {
            // Change the state and update the current point
            state->value = event.state;
            moving->isMoving = true;
            goToNextPoint(*moving, *position, *state);
        }
    }
    es::Events::clear<MovingEvent>();
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
    vectors::normalize(diff);

    // Multiply unit vector by speed to get velocity
    moving.velocity = diff * moving.speed;

    // Calculate the distance to the end point
    moving.distance = vectors::distance(start, end);

    // Save the starting position
    moving.startPos = sf::Vector2f(position.x, position.y);
}