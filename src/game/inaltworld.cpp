// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "inaltworld.h"
#include "components.h"

bool inAltWorld(es::Entity ent)
{
    // Check both the AltWorld and TilePosition components
    bool altWorld = ent.has<AltWorld>();
    if (!altWorld)
    {
        auto tilePos = ent.get<TilePosition>();
        if (tilePos)
            altWorld = (tilePos->layer != 0);
    }
    return altWorld;
}
