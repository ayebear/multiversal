// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "lasersystem.h"
#include "nage/graphics/spriteloader.h"
#include "tilemapdata.h"
#include "nage/graphics/tilemap.h"
#include "magicwindow.h"
#include "logicaltiles.h"
#include "nage/graphics/vectors.h"
#include "es/events.h"
#include "gameevents.h"
#include <cmath>

const char* LaserSystem::textureFilename = "data/images/beam.png";

LaserSystem::LaserSystem(ocs::ObjectManager& objects, TileMapData& tileMapData, ng::TileMap& tileMap, MagicWindow& magicWindow):
    objects(objects),
    tileMapData(tileMapData),
    tileMap(tileMap),
    magicWindow(magicWindow)
{
    ng::SpriteLoader::preloadTexture(textureFilename);
    auto& texture = ng::SpriteLoader::getTexture(textureFilename);
    texture.setSmooth(true);
    beamWidth = texture.getSize().x;
}

void LaserSystem::initialize()
{
    tileSize = tileMap.getTileSize();
    mapSize = tileMap.getMapSize();

    // Update the rotation components based on the direction of the lasers
    for (auto& laser: objects.getComponentArray<Components::Laser>())
    {
        auto rotation = objects.getComponent<Components::Rotation>(laser.getOwnerID());
        if (rotation)
            rotation->angle = laser.getAngle(laser.direction);
    }
}

void LaserSystem::update(float dt)
{
    // Store the laser sensors to turn off
    laserSensorsToDisable.clear();
    for (int tileId: tileMapData[Tiles::LaserSensor])
        laserSensorsToDisable.insert(tileId);

    // Update laser beams
    for (auto& laser: objects.getComponentArray<Components::Laser>())
    {
        auto tilePos = objects.getComponent<Components::TilePosition>(laser.getOwnerID());
        auto state = objects.getComponent<Components::State>(laser.getOwnerID());
        if (tilePos && state && state->value)
            addBeams(laser, *tilePos);
        else
            laser.beams.clear();
    }

    // Turn off the laser sensors in the list
    for (int tileId: laserSensorsToDisable)
        es::Events::send(SwitchEvent{tileId, SwitchEvent::Off});
}

LaserSystem::PointInfo LaserSystem::findPoint()
{
    PointInfo point;
    // Find a non-empty tile
    // Take into account the window and if it is active
    bool done = false;
    while (!done)
    {
        // Find the logical point first
        currentPosition.x += currentDirection.x;
        currentPosition.y += currentDirection.y;

        // Limit to boundaries
        if (currentPosition.x < 0 ||
            currentPosition.y < 0 ||
            currentPosition.x > int(mapSize.x) - 1 ||
            currentPosition.y > int(mapSize.y) - 1)
            break;

        // Get the layer of the current point depending on the window
        int layer = getLayer();
        if (currentLayer == 1)
            layer = 1;

        // Check if there is a laser colliding tile
        auto& tile = tileMapData(layer, currentPosition.x, currentPosition.y);
        if (tile.blocksLaser)
        {
            done = true;
            if (tile.logicalId == Tiles::LaserSensor)
                point.state = PointInfo::State::Activate;
            else if (tile.logicalId == Tiles::Mirror)
                point.state = PointInfo::State::Redirect;
            else
                point.state = PointInfo::State::Stop;
            point.tileId = tileMapData.getId(layer, currentPosition.x, currentPosition.y);
        }
    }

    // Calculate the graphical position
    point.position.x = currentPosition.x * int(tileSize.x);
    point.position.y = currentPosition.y * int(tileSize.y);
    if (point.state == PointInfo::State::Redirect)
    {
        // Set to center point
        point.position.x += (tileSize.x / 2);
        point.position.y += (tileSize.y / 2);
    }
    else
    {
        // Set to edge point (this depends on the current direction)
        point.position.x += float(tileSize.x) * ((currentDirection.x - 1) / -2.0f);
        point.position.y += float(tileSize.y) * ((currentDirection.y - 1) / -2.0f);
    }

    // TODO: Also collide with objects
    return point;
}

void LaserSystem::addBeams(Components::Laser& laser, Components::TilePosition& tilePos)
{
    // Setup everything
    currentPosition = ng::vectors::cast<int>(tilePos.pos);
    currentDirection = laser.direction;

    // Current layer is the starting layer of the beam
    currentLayer = (tilePos.layer && !getLayer());

    // Simulate the laser until it hits something
    laser.beamCount = 0;
    sf::Vector2f startPoint = tileMap.getCenterPoint<float>(tilePos.pos);
    PointInfo endPoint;
    do
    {
        // Find next point that collides
        endPoint = findPoint();

        Components::Laser::Beam* beam;
        if (laser.beamCount < laser.beams.size())
        {
            // Use an existing sprite
            beam = &(laser.beams[laser.beamCount]);
        }
        else
        {
            // Setup a new sprite with the beam texture
            laser.beams.emplace_back();
            beam = &laser.beams.back();
            ng::SpriteLoader::load(beam->sprite, textureFilename, true);
        }
        ++(laser.beamCount);

        beam->sprite.setOrigin(beamWidth / 2, 0);

        // Calculate and set the beam length
        float beamLength = ng::vectors::distance(startPoint, endPoint.position);
        beam->sprite.setScale(1, beamLength);

        // Set the beam's position and rotation
        beam->sprite.setPosition(startPoint);
        beam->sprite.setRotation(ng::vectors::rotateAngle(laser.getAngle(currentDirection), 180.0));

        // Set the beam's layer
        beam->layer = currentLayer;

        if (endPoint.state == PointInfo::State::Redirect)
        {
            // Change the direction based on the angle of the mirror
            bool mirrorState = tileMapData(endPoint.tileId).state;
            changeDirection(mirrorState, currentDirection);
        }
        else if (endPoint.state == PointInfo::State::Activate)
        {
            // Enable the laser sensor
            es::Events::send(SwitchEvent{endPoint.tileId, SwitchEvent::On});
            laserSensorsToDisable.erase(endPoint.tileId);
        }

        startPoint = endPoint.position;
    }
    while (endPoint.state == PointInfo::State::Redirect);
}

int LaserSystem::getLayer() const
{
    auto pos = tileMap.getCenterPoint<unsigned>(currentPosition);
    return (magicWindow.isWithin(pos) ? 1 : 0);
}

void LaserSystem::changeDirection(bool state, sf::Vector2i& direction) const
{
    // Used for redirecting the beam
    std::swap(direction.x, direction.y);
    if (!state)
    {
        direction.x *= -1;
        direction.y *= -1;
    }
}
