// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef OBJECTSWITCHSYSTEM_H
#define OBJECTSWITCHSYSTEM_H

#include "es/system.h"
#include "level.h"

/*
Changes the state of world connected to switches.
Receives switch output events to work.
*/
class ObjectSwitchSystem: public es::System
{
    public:
        ObjectSwitchSystem(Level& level, es::World& world);
        void update(float dt);

    private:
        void toggleObjectState(const std::string& name);

        Level& level;
        es::World& world;
};

#endif
