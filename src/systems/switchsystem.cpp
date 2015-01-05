// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "switchsystem.h"
#include "tilemapdata.h"
#include "tilemapchanger.h"
#include "logicaltiles.h"
#include "events.h"

SwitchSystem::SwitchSystem(TileMapData& tileMapData, TileMapChanger& tileMapChanger):
    tileMapData(tileMapData),
    tileMapChanger(tileMapChanger)
{
}

void SwitchSystem::update(float dt)
{
    es::Events::clear<SwitchOutputEvent>();

    // Get the initial switch map
    for (auto& event: es::Events::get<SwitchMapEvent>())
        switches.swap(event.switches);
    es::Events::clear<SwitchMapEvent>();

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
}

void SwitchSystem::flipSwitch(int tileId, bool state)
{
    // If the state of the switch itself changed,
    // then change everything connected to the switch
    if (tileMapChanger.changeState(tileId, state))
    {
        // Create a switch output event
        SwitchOutputEvent event;
        for (int id: switches[tileId])
            event.tileIds[tileMapData(id).logicalId].push_back(id);

        // Send the event
        es::Events::send(event);
    }
}
