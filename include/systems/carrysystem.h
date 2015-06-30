// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef CARRYSYSTEM_H
#define CARRYSYSTEM_H

#include "es/system.h"
#include "es/world.h"

class MagicWindow;

/*
This class handles entities that can "carry" other world.
TODO: Make this more generic to have an "attacher" that overrides positions.
*/
class CarrySystem: public es::System
{
    public:
        CarrySystem(es::World& world, MagicWindow& magicwindow);
        void update(float dt);

    private:
        es::World& world;
        MagicWindow& magicwindow;
};

#endif
