// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "carrysystem.h"
#include "components.h"
#include "es/events.h"
#include "gameevents.h"
#include "magicwindow.h"
#include <iostream>

CarrySystem::CarrySystem(es::World& world, MagicWindow& magicwindow):
    world(world),
    magicwindow(magicwindow)
{
}

void CarrySystem::update(float dt)
{
    // Process events, and check if anything should be picked up/put down
    for (auto& event: es::Events::get<ActionKeyEvent>())
    {
        auto ent = world[event.entityId];
        auto carrier = ent.get<Carrier>();
        auto aabb = ent.get<AABB>();
        if (carrier && aabb)
        {
            if (carrier->carrying)
            {
                // Calculate which world to place the object in
                auto carriedEnt = world[carrier->id];
                auto carriedPos = carriedEnt.get<Position>();
                auto carriedAabb = carriedEnt.get<AABB>();
                if (carriedAabb && magicwindow.isWithin(carriedAabb->getGlobalBounds(carriedPos.get())))
                    carriedEnt.assign<AltWorld>();

                // Drop object
                carriedEnt.remove<DrawOnTop>();
                carrier->carrying = false;
            }
            else if (!aabb->collisions.empty())
            {
                // Pickup object
                auto id = aabb->collisions.front();
                auto carriedEnt = world[id];
                if (carriedEnt.has<Carryable>())
                {
                    carrier->id = id;
                    carrier->carrying = true;
                    carriedEnt.remove<AltWorld, TilePosition>();
                    carriedEnt.assign<DrawOnTop>();
                }
            }
        }
    }

    // Update positions of world being carried
    for (auto ent: world.query<Carrier, Position>())
    {
        auto carrier = ent.get<Carrier>();
        if (carrier->carrying)
        {
            auto carrierPos = ent.get<Position>();
            auto carryableEnt = world[carrier->id];
            auto carryablePos = carryableEnt.get<Position>();
            if (carrierPos && carryablePos)
            {
                // Update position
                carryablePos->x = carrierPos->x + carrier->offset.x;
                carryablePos->y = carrierPos->y + carrier->offset.y;

                // Reset velocity
                if (carryableEnt.has<Velocity>())
                    carryableEnt.assign<Velocity>();
            }
        }
    }
}
