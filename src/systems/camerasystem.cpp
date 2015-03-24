// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "camerasystem.h"
#include "nage/graphics/camera.h"
#include "events.h"
#include "gameevents.h"
#include "nage/graphics/tilemap.h"
#include <iostream>

CameraSystem::CameraSystem(Camera& camera, TileMap& tileMap):
    camera(camera),
    tileMap(tileMap)
{
}

void CameraSystem::initialize()
{
    // Update the zoom amount of the views
    float viewHeight = camera.accessView("game").getSize().y;
    float levelHeight = tileMap.getPixelSize().y;
    float zoomAmount = levelHeight / viewHeight;
    std::cout << "Zoom: " << zoomAmount << " = " << levelHeight << " / " << viewHeight << "\n";

    // Update zoom amounts
    camera.accessView("game").zoom(zoomAmount);
    camera.accessView("background").zoom(zoomAmount);
    camera.accessView("game2").zoom(zoomAmount);
    camera.accessView("background2").zoom(zoomAmount);
}

void CameraSystem::update(float dt)
{
    // Receive camera events
    for (auto& event: es::Events::get<CameraEvent>())
    {
        // Set view's center based on entity's position and size
        auto playerPos = event.position;
        playerPos.x += event.size.x / 2;
        playerPos.y += event.size.y / 2;
        sf::Vector2f viewCenter(playerPos.x, playerPos.y);
        sf::Vector2f viewSize(camera.getView("game").getSize());

        // Make sure view isn't off the map
        sf::Vector2f halfSize(viewSize.x / 2, viewSize.y / 2);
        if (viewCenter.x - halfSize.x < 0)
            viewCenter.x = halfSize.x;
        if (viewCenter.y - halfSize.y < 0)
            viewCenter.y = halfSize.y;
        auto mapSize = tileMap.getPixelSize();
        if (viewCenter.x >= mapSize.x - halfSize.x)
            viewCenter.x = mapSize.x - halfSize.x;
        if (viewCenter.y >= mapSize.y - halfSize.y)
            viewCenter.y = mapSize.y - halfSize.y;

        camera.setCenter(viewCenter);
    }
}
