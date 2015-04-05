// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "inaltworld.h"
#include "components.h"

bool inAltWorld(ocs::ObjectManager& objects, ocs::ID id)
{
    // Check both the AltWorld and TilePosition components
    bool altWorld = objects.hasComponents<Components::AltWorld>(id);
    if (!altWorld)
    {
        auto tilePos = objects.getComponent<Components::TilePosition>(id);
        if (tilePos)
            altWorld = (tilePos->layer != 0);
    }
    return altWorld;
}
