// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MOVINGSYSTEM_H
#define MOVINGSYSTEM_H

#include "es/system.h"
#include "OCS/Objects.hpp"
#include "components.h"
#include "movingcomponent.h"

/*
Handles updating the positions of objects with Movable components.
Instead of using velocity/gravity components to update position like the physics system,
    this uses moving components.
*/
class MovingSystem: public es::System
{
    public:
        MovingSystem(ocs::ObjectManager& objects);
        void initialize();
        void update(float dt);

    private:
        void goToNextPoint(Components::Moving& moving, Components::Position& position, Components::State& state) const;
        void calculateVelocity(Components::Moving& moving, Components::Position& position) const;

        ocs::ObjectManager& objects;
};

#endif
