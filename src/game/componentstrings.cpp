// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "componentstrings.h"
#include "OCS/Objects.hpp"
#include "components.h"
#include "movingcomponent.h"
#include "lasercomponent.h"
#include <iostream>

void bindComponentStrings(ocs::ObjectManager& objects)
{
    std::cout << "Binding component strings...\n";
    BIND_COMP_STR(Position);
    BIND_COMP_STR(Velocity);
    BIND_COMP_STR(Gravity);
    BIND_COMP_STR(Size);
    BIND_COMP_STR(AABB);
    BIND_COMP_STR(Sprite);
    BIND_COMP_STR(AnimSprite);
    BIND_COMP_STR(Jumpable);
    BIND_COMP_STR(ObjectState);
    BIND_COMP_STR(Movable);
    BIND_COMP_STR(Carrier);
    BIND_COMP_STR(CameraUpdater);
    BIND_COMP_STR(AltWorld);
    BIND_COMP_STR(Carryable);
    BIND_COMP_STR(DrawOnTop);
    BIND_COMP_STR(AboveWindow);
    BIND_COMP_STR(Moving);
    BIND_COMP_STR(State);
    BIND_COMP_STR(TileGroup);
    BIND_COMP_STR(Laser);
    BIND_COMP_STR(TilePosition);
    BIND_COMP_STR(Rotation);
    BIND_COMP_STR(Switch);
    std::cout << "Done binding component strings.\n";
}
