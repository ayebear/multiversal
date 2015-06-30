// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilegroupsystem.h"
#include "components.h"
#include "tilemapchanger.h"
#include "es/world.h"

TileGroupSystem::TileGroupSystem(TileMapChanger& tileMapChanger, es::World& world):
    tileMapChanger(tileMapChanger),
    world(world)
{
}

void TileGroupSystem::initialize()
{
    for (auto& tileGroup: world.getComponents<TileGroup>())
    {
        for (auto id: tileGroup.tileIds)
            tileMapChanger.changeState(id, tileGroup.initialState);
    }
}

void TileGroupSystem::update(float dt)
{
    for (auto ent: world.query<TileGroup, State>())
    {
        auto tileGroup = ent.get<TileGroup>();
        auto state = ent.get<State>();
        if (state->hasChanged())
        {
            for (auto id: tileGroup->tileIds)
                tileMapChanger.toggleState(id);
        }
    }
}
