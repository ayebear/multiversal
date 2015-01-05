// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef FORCEFIELDSYSTEM_H
#define FORCEFIELDSYSTEM_H

#include "system.h"

class TileMapChanger;

/*
Handles enabling/disabling the force field tiles in the level.
*/
class ForceFieldSystem: public es::System
{
    public:
        ForceFieldSystem(TileMapChanger& tileMapChanger);
        void update(float dt);

    private:
        TileMapChanger& tileMapChanger;
};

#endif
