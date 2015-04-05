// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilegroupsystem.h"
#include "components.h"
#include "tilemapchanger.h"

TileGroupSystem::TileGroupSystem(TileMapChanger& tileMapChanger, ocs::ObjectManager& objects):
    tileMapChanger(tileMapChanger),
    objects(objects)
{
}

void TileGroupSystem::initialize()
{
    for (auto& tileGroup: objects.getComponentArray<Components::TileGroup>())
    {
        for (auto id: tileGroup.tileIds)
            tileMapChanger.changeState(id, tileGroup.initialState);
    }
}

void TileGroupSystem::update(float dt)
{
    for (auto& tileGroup: objects.getComponentArray<Components::TileGroup>())
    {
        auto state = objects.getComponent<Components::State>(tileGroup.getOwnerID());
        if (state && state->hasChanged())
        {
            for (auto id: tileGroup.tileIds)
                tileMapChanger.toggleState(id);
        }
    }
}
