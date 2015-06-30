// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "objectswitchsystem.h"
#include "es/events.h"
#include "gameevents.h"
#include "components.h"
#include <iostream>

ObjectSwitchSystem::ObjectSwitchSystem(Level& level, es::World& world):
    level(level),
    world(world)
{
}

void ObjectSwitchSystem::update(float dt)
{
    for (auto& event: es::Events::get<SwitchOutputEvent>())
        for (auto& name: event.objectNames)
            toggleObjectState(name);
}

void ObjectSwitchSystem::toggleObjectState(const std::string& name)
{
    // Update state component if it exists
    auto state = world.get(name).getPtr<State>();
    if (state)
        state->value = !state->value;
}
