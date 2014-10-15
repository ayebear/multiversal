// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "camerasystem.h"
#include "camera.h"
#include "events.h"
#include "gameevents.h"

CameraSystem::CameraSystem(Camera& camera):
    camera(camera)
{
}

void CameraSystem::update(float dt)
{
    // Receive map size events
    for (auto& event: Events::get<MapSizeEvent>())
        mapSize = event.mapSize;
    Events::clear<MapSizeEvent>();

    // Receive camera events
    for (auto& event: Events::get<CameraEvent>())
    {
        // Set view's center based on entity's position and size
        auto playerPos = event.position;
        playerPos.x += event.size.x / 2;
        playerPos.y += event.size.y / 2;
        sf::Vector2f viewCenter(playerPos.x, playerPos.y);
        sf::Vector2f viewSize(camera.getView("game").getSize());

        // Make sure view isn't off the map
        if (viewCenter.x - (viewSize.x / 2) < 0)
            viewCenter.x = viewSize.x / 2;
        if (viewCenter.y - (viewSize.y / 2) < 0)
            viewCenter.y = viewSize.y / 2;
        if (viewCenter.x >= mapSize.x - (viewSize.x / 2))
            viewCenter.x = mapSize.x - (viewSize.x / 2);
        if (viewCenter.y >= mapSize.y - (viewSize.y / 2))
            viewCenter.y = mapSize.y - (viewSize.y / 2);

        camera.setCenter(viewCenter);
    }
}
