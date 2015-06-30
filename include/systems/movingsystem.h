// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MOVINGSYSTEM_H
#define MOVINGSYSTEM_H

#include "components.h"
#include "movingcomponent.h"
#include "es/system.h"
#include "es/world.h"

/*
Handles updating the positions of world with Movable components.
Instead of using velocity/gravity components to update position like the physics system,
    this uses moving components.
*/
class MovingSystem: public es::System
{
    public:
        MovingSystem(es::World& world);
        void initialize();
        void update(float dt);

    private:
        void goToNextPoint(Moving& moving, Position& position, State& state) const;
        void calculateVelocity(Moving& moving, Position& position) const;

        es::World& world;
};

#endif
