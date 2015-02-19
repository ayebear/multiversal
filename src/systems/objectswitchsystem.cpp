// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "objectswitchsystem.h"
#include "events.h"
#include "gameevents.h"
#include "components.h"
#include <iostream>

ObjectSwitchSystem::ObjectSwitchSystem(Level& level, ocs::ObjectManager& entities):
    level(level),
    entities(entities)
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
    // Get object ID from name
    auto id = level.getObjectIdFromName(name);

    // Get state component from object ID
    auto state = entities.getComponent<Components::State>(id);

    // Update state
    if (state)
        state->value = !state->value;
}
