// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilesystem.h"
#include "tilemapdata.h"
#include "es/events.h"
#include "gameevents.h"
#include "logicaltiles.h"
#include "components.h"
#include "es/world.h"
#include <iostream>

TileSystem::TileSystem(es::World& world, TileMapData& tileMapData):
    world(world),
    tileMapData(tileMapData)
{
}

void TileSystem::update(float dt)
{
    // Handle action key events (when the player presses "up") on different tiles
    for (auto& event: es::Events::get<ActionKeyEvent>())
    {
        auto aabb = world[event.entityId].get<AABB>();
        if (aabb)
        {
            for (int tileId: aabb->tileCollisions)
            {
                // Get the logical ID
                int logicalId = tileMapData(tileId).logicalId;

                // Handle the action event depending on which tile type it is
                if (logicalId == Tiles::Exit)
                    handleExitTile();
                else if (logicalId == Tiles::ToggleSwitch)
                    es::Events::send(SwitchEvent{tileId, SwitchEvent::Toggle});
            }
        }
    }
}

void TileSystem::handleExitTile()
{
    if (!es::Events::exists<LoadNextLevelEvent>())
    {
        std::cout << "Sent LoadNextLevelEvent\n";
        es::Events::send(LoadNextLevelEvent());
    }
}
