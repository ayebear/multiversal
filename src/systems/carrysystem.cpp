#include "carrysystem.h"
#include "components.h"
#include "events.h"
#include "gameevents.h"
#include "magicwindow.h"
#include <iostream>

CarrySystem::CarrySystem(ocs::ObjectManager& entities, MagicWindow& magicwindow):
    entities(entities),
    magicwindow(magicwindow)
{
}

void CarrySystem::update()
{
    // Process events, and check if anything should be picked up/put down
    for (auto& event: Events::get<ActionKeyEvent>())
    {
        auto carrier = entities.getComponent<Components::Carrier>(event.entityId);
        auto aabb = entities.getComponent<Components::AABB>(event.entityId);
        if (carrier && aabb)
        {
            if (carrier->carrying)
            {
                // Drop

                /*
                // Set the velocity of the object being dropped to the same as the carrier's
                // NOTE: Disabled due to frictionless surfaces
                auto carrierVel = entities.getComponent<Components::Velocity>(event.entityId);
                auto carryableVel = entities.getComponent<Components::Velocity>(carrier->id);
                if (carrierVel && carryableVel)
                {
                    carryableVel->x = carrierVel->x;
                    carryableVel->y = carrierVel->y;
                }
                */

                // Calculate which world to place the object in
                auto carriedPos = entities.getComponent<Components::Position>(carrier->id);
                auto carriedAabb = entities.getComponent<Components::AABB>(carrier->id);
                if (carriedAabb && magicwindow.isWithin(carriedAabb->getGlobalBounds(carriedPos)))
                    entities.addComponents(carrier->id, Components::AltWorld());

                entities.removeComponents<Components::DrawOnTop>(carrier->id);
                carrier->carrying = false;
                std::cout << "Put down object!\n";
            }
            else if (!aabb->collisions.empty())
            {
                // Pickup
                auto id = aabb->collisions.front();
                auto carryable = entities.getComponent<Components::Carryable>(id);
                if (carryable)
                {
                    carrier->id = id;
                    entities.removeComponents<Components::AltWorld>(id);
                    entities.addComponents(id, Components::DrawOnTop());
                    carrier->carrying = true;
                    std::cout << "Picked up object " << id << ". # collisions = " << aabb->collisions.size() << "\n";
                }
            }
            else
                std::cout << "No collisions?\n";
        }
    }

    // Update positions of entities being carried
    for (auto& carrier: entities.getComponentArray<Components::Carrier>())
    {
        if (carrier.carrying)
        {
            auto carrierPos = entities.getComponent<Components::Position>(carrier.getOwnerID());
            auto carryablePos = entities.getComponent<Components::Position>(carrier.id);
            if (carrierPos && carryablePos)
            {
                carryablePos->x = carrierPos->x + carrier.offset.x;
                carryablePos->y = carrierPos->y + carrier.offset.y;
                auto velocity = entities.getComponent<Components::Velocity>(carrier.id);
                if (velocity)
                {
                    velocity->x = 0;
                    velocity->y = 0;
                }
            }
        }
    }
}
