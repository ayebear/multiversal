// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "componentstrings.h"
#include "OCS/Objects.hpp"
#include "components.h"
#include "movingcomponent.h"
#include "lasercomponent.h"
#include <iostream>

#define bindCompName(name) objects.bindStringToComponent<Components::name>(#name)

void bindComponentStrings(ocs::ObjectManager& objects)
{
    std::cout << "Binding component strings...\n";
    bindCompName(Position);
    bindCompName(Velocity);
    bindCompName(Gravity);
    bindCompName(Size);
    bindCompName(AABB);
    bindCompName(Sprite);
    bindCompName(AnimSprite);
    bindCompName(Jumpable);
    bindCompName(ObjectState);
    bindCompName(Movable);
    bindCompName(Carrier);
    bindCompName(CameraUpdater);
    bindCompName(AltWorld);
    bindCompName(Carryable);
    bindCompName(DrawOnTop);
    bindCompName(AboveWindow);
    bindCompName(Moving);
    bindCompName(State);
    bindCompName(TileGroup);
    bindCompName(Laser);
    bindCompName(TilePosition);
    bindCompName(Rotation);
    bindCompName(Switch);
    bindCompName(InitialPosition);
    std::cout << "Done binding component strings.\n";
}
