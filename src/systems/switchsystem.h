// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SWITCHSYSTEM_H
#define SWITCHSYSTEM_H

#include "system.h"
#include "gameevents.h"

class TileMapData;
class TileMapChanger;

/*
Handles the input and output of the switches on the tile map.
May make separate classes for the different switch types.
*/
class SwitchSystem: public es::System
{
    public:
        SwitchSystem(TileMapData& tileMapData, TileMapChanger& tileMapChanger);
        void update(float dt);

    private:
        void flipSwitch(int tileId, bool state);

        SwitchMap switches;

        TileMapData& tileMapData;
        TileMapChanger& tileMapChanger;
};

#endif
