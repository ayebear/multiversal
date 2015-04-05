// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "carrysystem.h"
#include "components.h"
#include "events.h"
#include "gameevents.h"
#include "magicwindow.h"
#include <iostream>

CarrySystem::CarrySystem(ocs::ObjectManager& objects, MagicWindow& magicwindow):
    objects(objects),
    magicwindow(magicwindow)
{
}

void CarrySystem::update(float dt)
{
    // Process events, and check if anything should be picked up/put down
    for (auto& event: es::Events::get<ActionKeyEvent>())
    {
        auto carrier = objects.getComponent<Components::Carrier>(event.entityId);
        auto aabb = objects.getComponent<Components::AABB>(event.entityId);
        if (carrier && aabb)
        {
            if (carrier->carrying)
            {
                // Calculate which world to place the object in
                auto carriedPos = objects.getComponent<Components::Position>(carrier->id);
                auto carriedAabb = objects.getComponent<Components::AABB>(carrier->id);
                if (carriedAabb && magicwindow.isWithin(carriedAabb->getGlobalBounds(carriedPos)))
                    objects.addComponents(carrier->id, Components::AltWorld());

                // Drop object
                objects.removeComponents<Components::DrawOnTop>(carrier->id);
                carrier->carrying = false;
            }
            else if (!aabb->collisions.empty())
            {
                // Pickup object
                auto id = aabb->collisions.front();
                auto carryable = objects.getComponent<Components::Carryable>(id);
                if (carryable)
                {
                    carrier->id = id;
                    objects.removeComponents<Components::AltWorld, Components::TilePosition>(id);
                    objects.addComponents(id, Components::DrawOnTop());
                    carrier->carrying = true;
                }
            }
        }
    }

    // Update positions of objects being carried
    for (auto& carrier: objects.getComponentArray<Components::Carrier>())
    {
        if (carrier.carrying)
        {
            auto carrierPos = objects.getComponent<Components::Position>(carrier.getOwnerID());
            auto carryablePos = objects.getComponent<Components::Position>(carrier.id);
            if (carrierPos && carryablePos)
            {
                // Update position
                carryablePos->x = carrierPos->x + carrier.offset.x;
                carryablePos->y = carrierPos->y + carrier.offset.y;

                // Reset velocity
                auto velocity = objects.getComponent<Components::Velocity>(carrier.id);
                if (velocity)
                {
                    velocity->x = 0;
                    velocity->y = 0;
                }
            }
        }
    }
}
