// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "forcefieldsystem.h"
#include "tilemapchanger.h"
#include "events.h"
#include "gameevents.h"
#include "logicaltiles.h"

ForceFieldSystem::ForceFieldSystem(TileMapChanger& tileMapChanger):
    tileMapChanger(tileMapChanger)
{
}

void ForceFieldSystem::update(float dt)
{
    // Iterate through the events
    for (auto& event: es::Events::get<SwitchOutputEvent>())
    {
        // Only handle force field tile changes
        auto found = event.tileIds.find(Tiles::ForceField);
        if (found != event.tileIds.end())
        {
            // Iterate through the tile IDs of the force field tiles
            for (int tileId: found->second)
                tileMapChanger.toggleState(tileId);
        }
    }
}
