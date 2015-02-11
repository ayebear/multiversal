#ifndef MOVINGSYSTEM_H
#define MOVINGSYSTEM_H

#include "system.h"
#include "OCS/Objects.hpp"
#include "components.h"

/*
Handles updating the positions of objects with Movable components.
Instead of using gravity components to update velocity like the physics system,
    this uses moving components.
*/
class MovingSystem: public es::System
{
    public:
        MovingSystem(ocs::ObjectManager& entities);
        void update(float dt);

    private:
        void processEvents();
        void goToNextPoint(Components::Moving& moving, Components::Position& position) const;
        void calculateVelocity(Components::Moving& moving, Components::Position& position) const;

        ocs::ObjectManager& entities;
};

#endif
