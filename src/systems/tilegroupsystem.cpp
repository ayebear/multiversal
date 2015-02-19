// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilegroupsystem.h"
#include "components.h"
#include "tilemapchanger.h"

TileGroupSystem::TileGroupSystem(TileMapChanger& tileMapChanger, ocs::ObjectManager& entities):
    tileMapChanger(tileMapChanger),
    entities(entities)
{
}

void TileGroupSystem::update(float dt)
{
    for (auto& tileGroup: entities.getComponentArray<Components::TileGroup>())
    {
        auto state = entities.getComponent<Components::State>(tileGroup.getOwnerID());
        if (state && state->hasChanged())
        {
            for (auto id: tileGroup.tileIds)
                tileMapChanger.toggleState(id);
        }
    }
}
