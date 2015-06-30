// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "physicssystem.h"
#include "tilemapdata.h"
#include "nage/graphics/tilemap.h"
#include "magicwindow.h"
#include "components.h"
#include "es/events.h"
#include "gameevents.h"
#include "inaltworld.h"
#include "level.h"
#include "nage/misc/utils.h"
#include "nage/graphics/vectors.h"
#include <iostream>

const sf::Vector2f PhysicsSystem::maxVelocity(3200, 3200);
const sf::Vector2f PhysicsSystem::gravityConstant(640, 640);

PhysicsSystem::PhysicsSystem(es::World& world, TileMapData& tileMapData, ng::TileMap& tileMap, MagicWindow& magicWindow, Level& level):
    world(world),
    tileMapData(tileMapData),
    tileMap(tileMap),
    magicWindow(magicWindow),
    level(level)
{
}

void PhysicsSystem::initialize()
{
    updateTilePositionComponents();
}

void PhysicsSystem::update(float dt)
{
    stepPositions(dt);
    checkEntityCollisions();
    checkTileCollisions();

    // Print tile locations for debugging
    for (auto& event: es::Events::get<MouseClickedEvent>())
    {
        const auto& tileSize = tileMap.getTileSize();
        sf::Vector2i location(event.mousePos.x / tileSize.x, event.mousePos.y / tileSize.y);
        int tileId1 = tileMapData.getId(0, location.x, location.y);
        int tileId2 = tileMapData.getId(1, location.x, location.y);
        std::cout << "Location: (" << location.x << ", " << location.y << "), ID: " << tileId1 << ", AltID: " << tileId2 << "\n";
    }
}

void PhysicsSystem::stepPositions(float dt)
{
    tileMapData.clearTiles();

    // Clear events
    es::Events::clear<CameraEvent>();

    // Apply gravity and handle collisions
    for (auto ent: world.query<Velocity, Position, Size>())
    {
        auto entityId = ent.getId();
        auto velocity = ent.get<Velocity>();
        auto position = ent.get<Position>();
        auto size = ent.get<Size>();

        // Apply gravity from the gravity component (if it exists)
        auto gravity = ent.get<Gravity>();
        if (gravity)
        {
            velocity->x += dt * gravity->acceleration.x * gravityConstant.x;
            velocity->y += dt * gravity->acceleration.y * gravityConstant.y;
            ng::clamp(velocity->x, -maxVelocity.x, maxVelocity.x);
            ng::clamp(velocity->y, -maxVelocity.y, maxVelocity.y);
        }

        // Get the AABB component used for collision detection/handling
        auto aabb = ent.get<AABB>();

        // See if the entity is in the alternate world!
        bool altWorld = inAltWorld(ent);

        // Update Y
        position->y += dt * velocity->y;
        if (aabb)
            handleTileCollision(aabb.get(), velocity->y, position.get(), true, entityId, altWorld);

        // Update X
        position->x += dt * velocity->x;
        if (aabb)
            handleTileCollision(aabb.get(), velocity->x, position.get(), false, entityId, altWorld);

        // Fix edge cases
        updateEdgeCases(position.get(), size.get(), velocity->y, entityId);

        // Send camera update event
        if (ent.has<CameraUpdater>())
            es::Events::send(CameraEvent{sf::Vector2f(position->x, position->y),
                                         sf::Vector2f(size->x, size->y)});
    }
}

void PhysicsSystem::handleTileCollision(AABB* entAABB, float& velocity, Position* position, bool vertical, es::ID entityId, bool altWorld)
{
    // Object - tile map collision
    // Need to send an event when this happens so the entity knows which tile it is colliding with
    // And it would send what kind of event: falling off platform, standing on platform, etc.

    // Temp computed AABB (position + AABB component)
    auto tempAABB = entAABB->getGlobalBounds(position);
    float newTop = tempAABB.top;

    // Get the area of tiles to check collision against
    sf::Vector2u start;
    sf::Vector2u end;
    getCollidingTiles(tempAABB, start, end);

    auto ent = world[entityId];
    bool aboveWindow = ent.has<AboveWindow>();

    // Check the collision
    bool onPlatform = false;
    const auto& tileSize = tileMap.getTileSize();
    for (unsigned y = start.y; y <= end.y; ++y)
    {
        for (unsigned x = start.x; x <= end.x; ++x)
        {
            // Find which layer this object is part of
            int layer = determineLayer(altWorld, true, x, y);
            tileMapData.useLayer(layer);
            if (tileMapData(x, y).collidable)
            {
                auto tileBox = tileMap.getBoundingBox(x, y);
                if (tileBox.intersects(tempAABB))
                {
                    if (vertical) // Use y
                    {
                        if (velocity >= 0) // Object is on platform
                        {
                            newTop = y * tileSize.y - tempAABB.height;
                            onPlatform = true;

                            // Add the tile ID to the set of tiles with world on them
                            int newLayer = determineLayer(altWorld, aboveWindow, x, y - 1);
                            tileMapData.addTile(tileMapData.getId(newLayer, x, y - 1));
                        }
                        else // Hitting ceiling
                            tempAABB.top = (y + 1) * tileSize.y;
                    }
                    else // Use x
                    {
                        if (velocity >= 0) // Moving to the right
                            tempAABB.left = x * tileSize.x - tempAABB.width;
                        else // Moving to the left
                            tempAABB.left = (x + 1) * tileSize.x;
                    }
                    velocity = 0;
                }
            }
        }
    }

    // Set the position of the entity
    if (onPlatform)
        tempAABB.top = newTop;

    // Notify the entity about it being in the air or on a platform
    if (vertical && ent)
        updateOnPlatformState(entityId, onPlatform ? ObjectState::OnPlatform : ObjectState::InAir);

    // Update the position from the new AABB
    position->x = tempAABB.left - entAABB->rect.left;
    position->y = tempAABB.top - entAABB->rect.top;
}

void PhysicsSystem::updateEdgeCases(Position* position, Size* size, float& velocity, es::ID entityId)
{
    auto levelSize = tileMap.getPixelSize();
    sf::Vector2i newPosition(levelSize.x - size->x, levelSize.y - size->y);

    // Ensure that entities won't move outside of the level
    ng::clampLT(position->x, 0);
    if (ng::clampLT(position->y, 0))
        velocity = 0;
    ng::clampGT(position->x, newPosition.x);
    if (ng::clampGT(position->y, newPosition.y))
        updateOnPlatformState(entityId, ObjectState::OnPlatform);
}

void PhysicsSystem::checkEntityCollisions()
{
    // TODO: Add "rigid" properties: Completely solid, and collidable only on the top
    // TODO: Handle collisions between collidable (rigid) components
    // TODO: Use a quad-tree or some spatial partitioning to improve performance

    // Use an O(n^2) algorithm to detect collisions between entities
    // Update the collision lists on any colliding components
    auto entities = world.query<AABB, Position>();
    for (auto& ent1: entities)
    {
        auto& aabb = *ent1.getPtr<AABB>();
        aabb.collisions.clear();
        for (auto& ent2: entities)
        {
            if (ent1.getId() != ent2.getId())
            {
                // TODO: Dynamically build a new world when moving the window
                    // This will make things 100% accurate and much simpler (no crazy boolean logic like below)

                auto& aabb2 = *ent2.getPtr<AABB>();
                auto pos = ent1.get<Position>();
                auto pos2 = ent2.get<Position>();

                bool drawOnTop = ent1.has<DrawOnTop>();
                bool drawOnTop2 = ent2.has<DrawOnTop>();
                bool altWorld = inAltWorld(ent1);
                bool altWorld2 = inAltWorld(ent2);
                bool inWindow = magicWindow.isWithin(aabb.getGlobalBounds(pos.get()));
                bool inWindow2 = magicWindow.isWithin(aabb2.getGlobalBounds(pos2.get()));

                bool case1 = (altWorld == inWindow && altWorld2 == inWindow2); // "Real world"
                bool case2 = ((altWorld && !inWindow) && (altWorld2 && !inWindow2)); // "Alternate world"
                bool case3 = (!altWorld && drawOnTop && inWindow && altWorld2 && inWindow2); // An object on top, like the player
                bool case4 = (!altWorld2 && drawOnTop2 && inWindow2 && altWorld && inWindow); // Another object on top
                bool case5 = (!altWorld && drawOnTop && inWindow && !altWorld2 && drawOnTop2 && inWindow2); // Both world on top and in window

                if (case1 || case2 || case3 || case4 || case5)
                {
                    if (aabb.getGlobalBounds(pos.get()).intersects(aabb2.getGlobalBounds(pos2.get())))
                        aabb.collisions.push_back(ent2.getId());
                }
            }
        }
    }
}

void PhysicsSystem::checkTileCollisions()
{
    // Generate lists of tile coordinates colliding with AABB components
    // TODO: This may only be useful for the player, so make an optional flag or component to enable it
    for (auto ent: world.query<AABB, Position>())
    {
        auto aabb = ent.get<AABB>();
        auto position = ent.get<Position>();
        aabb->tileCollisions.clear();
        if (position)
        {
            sf::Vector2u start, end;
            auto globalBounds = aabb->getGlobalBounds(position.get());
            getCollidingTiles(globalBounds, start, end);
            bool inWindow = magicWindow.isWithin(globalBounds);
            for (unsigned y = start.y; y <= end.y; ++y)
            {
                for (unsigned x = start.x; x <= end.x; ++x)
                {
                    // Figure out which layer the tile is in
                    int layer = (inWindow && magicWindow.isWithin(tileMap.getCenterPoint<unsigned>(x, y)));
                    int tileId = tileMapData.getId(layer, x, y);

                    // Add the tile ID to the collision list
                    if (tileMapData(tileId).logicalId > 0)
                        aabb->tileCollisions.push_back(tileId);
                }
            }
        }
    }
}

int PhysicsSystem::determineLayer(bool altWorld, bool aboveWindow, unsigned x, unsigned y) const
{
    return (altWorld || (aboveWindow && magicWindow.isWithin(tileMap.getCenterPoint<unsigned>(x, y))));
}

void PhysicsSystem::updateTilePositionComponents()
{
    // Initialize tile positions from InitialPosition components
    for (auto srcEnt: world.query<InitialPosition>())
    {
        auto name = srcEnt.get<InitialPosition>()->objectName;
        if (!name.empty())
        {
            // Update tile position from initial position's object's tile position
            auto destEnt = world[name];
            auto src = srcEnt.at<TilePosition>();
            auto dest = destEnt.at<TilePosition>();
            dest->id = src->id;
        }
    }

    // Initial positions from TilePosition components
    for (auto ent: world.query<TilePosition>())
    {
        // Update pos/layer
        auto& tilePos = *ent.getPtr<TilePosition>();
        tilePos.layer = tileMapData.getLayer(tilePos.id);
        tilePos.pos.x = tileMapData.getX(tilePos.id);
        tilePos.pos.y = tileMapData.getY(tilePos.id);

        auto position = ent.get<Position>();

        if (ent.has<Rotation>())
        {
            // Use the center if there is a rotation component
            // Note: Physics and collisions won't work properly with this

            // Update position components if they exist
            if (position)
            {
                auto center = tileMap.getCenterPoint<float>(tilePos.pos.x, tilePos.pos.y);
                position->x = center.x;
                position->y = center.y;
            }

            // Update sprite components to have a centered origin point
            // Note: This is done so rotations will work as expected
            auto sprite = ent.get<Sprite>();
            if (sprite)
            {
                auto bounds = sprite->sprite.getGlobalBounds();
                sprite->sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
            }
        }
        else
        {
            // Use the top-left like normal if there is no rotation component
            if (position)
            {
                auto bounds = tileMap.getBoundingBox(tilePos.pos.x, tilePos.pos.y);
                position->x = bounds.left;
                position->y = bounds.top;
            }
        }
    }
}

void PhysicsSystem::updateOnPlatformState(es::ID entityId, int state)
{
    auto objectState = world[entityId].get<ObjectState>();
    if (objectState)
        objectState->state = state;
}

void PhysicsSystem::getCollidingTiles(const sf::FloatRect& entAABB, sf::Vector2u& start, sf::Vector2u& end)
{
    const auto& tileSize = tileMap.getTileSize();
    const auto& mapSize = tileMap.getMapSize();

    // Get the area to check collision against
    auto startSigned = sf::Vector2i(entAABB.left / tileSize.x,
                                    entAABB.top / tileSize.y);
    end = sf::Vector2u((entAABB.left + entAABB.width) / tileSize.x,
                       (entAABB.top + entAABB.height) / tileSize.y);

    // Make sure this is within bounds
    ng::clampLT(startSigned.x, 0);
    ng::clampLT(startSigned.y, 0);
    ng::clampGT(end.x, mapSize.x - 1);
    ng::clampGT(end.y, mapSize.y - 1);

    start = ng::vec::cast<unsigned>(startSigned);
}
