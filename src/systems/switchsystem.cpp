// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "switchsystem.h"
#include "tilemapdata.h"
#include "tilemapchanger.h"
#include "logicaltiles.h"
#include "es/events.h"
#include "components.h"
#include <iostream>

SwitchSystem::SwitchSystem(TileMapData& tileMapData, TileMapChanger& tileMapChanger, ocs::ObjectManager& objects):
    tileMapData(tileMapData),
    tileMapChanger(tileMapChanger),
    objects(objects)
{
}

void SwitchSystem::initialize()
{
    // Populate switch objects map from Switch components
    switchObjects.clear();
    for (auto& switchComp: objects.getComponentArray<Components::Switch>())
        switchObjects[switchComp.tileId] = switchComp.objectNames;
}

void SwitchSystem::update(float dt)
{
    es::Events::clear<SwitchOutputEvent>();

    // Update push-button switches
    for (int tileId: tileMapData[Tiles::PushButton])
    {
        // Check if an object is on top of this tile
        bool objectOnTop = tileMapData.findTile(tileId);

        // If it is, then keep the switch on
        flipSwitch(tileId, objectOnTop);
    }

    // Update toggle switches, and anything else controlled by switch events
    for (auto& event: es::Events::get<SwitchEvent>())
    {
        // Determine the state from the action
        bool state;
        if (event.action == SwitchEvent::On)
            state = true;
        else if (event.action == SwitchEvent::Off)
            state = false;
        else
            state = !tileMapData(event.tileId).state;

        // Change the state of the switch
        flipSwitch(event.tileId, state);
    }
    es::Events::clear<SwitchEvent>();
}

void SwitchSystem::flipSwitch(int tileId, bool state)
{
    // If the state of the switch itself changed,
    // then change everything connected to the switch
    if (tileMapChanger.changeState(tileId, state))
    {
        // Send a switch output event
        SwitchOutputEvent event;
        event.objectNames = switchObjects[tileId];
        es::Events::send(event);

        std::cout << "Flipped switch " << tileId << ".\n";
    }
}
