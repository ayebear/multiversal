// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "physicssystem.h"
#include "tilemapdata.h"
#include "tilemap.h"
#include "magicwindow.h"
#include "components.h"
#include "events.h"
#include "gameevents.h"

const sf::Vector2i PhysicsSystem::maxVelocity(400, 800);
const sf::Vector2i PhysicsSystem::gravityConstant(160, 160);

PhysicsSystem::PhysicsSystem(ocs::ObjectManager& entities, TileMapData& tileMapData, TileMap& tileMap, MagicWindow& magicWindow):
    entities(entities),
    tileMapData(tileMapData),
    tileMap(tileMap),
    magicWindow(magicWindow)
{
}

void PhysicsSystem::update(float dt)
{
    stepPositions(dt);
    checkEntityCollisions();
    checkTileCollisions();
}

void PhysicsSystem::stepPositions(float dt)
{
    tileMapData.clearTiles();

    // Clear events
    es::Events::clear<CameraEvent>();
    es::Events::clear<OnPlatformEvent>();

    // Apply gravity and handle collisions
    for (auto& velocity: entities.getComponentArray<Components::Velocity>())
    {
        auto entityId = velocity.getOwnerID();
        auto position = entities.getComponent<Components::Position>(entityId);
        auto size = entities.getComponent<Components::Size>(entityId);
        if (position && size)
        {
            // Apply gravity from the gravity component (if it exists)
            auto gravity = entities.getComponent<Components::Gravity>(entityId);
            if (gravity)
            {
                velocity.x += dt * gravity->acceleration.x * gravityConstant.x;
                velocity.y += dt * gravity->acceleration.y * gravityConstant.y;
                if (velocity.x > maxVelocity.x)
                    velocity.x = maxVelocity.x;
                if (velocity.y > maxVelocity.y)
                    velocity.y = maxVelocity.y;
            }

            // Get the AABB component used for collision detection/handling
            auto aabb = entities.getComponent<Components::AABB>(entityId);

            // See if the entity is in the alternate world!
            bool inAltWorld = entities.hasComponents<Components::AltWorld>(entityId);

            // Update Y
            position->y += dt * velocity.y;
            if (aabb)
                handleTileCollision(aabb, velocity.y, position, true, entityId, inAltWorld);

            // Update X
            position->x += dt * velocity.x;
            if (aabb)
                handleTileCollision(aabb, velocity.x, position, false, entityId, inAltWorld);

            // Fix edge cases
            updateEdgeCases(position, size, velocity.y, entityId);

            // Send camera update event
            if (entities.hasComponents<Components::CameraUpdater>(velocity.getOwnerID()))
                es::Events::send(CameraEvent{sf::Vector2f(position->x, position->y),
                                             sf::Vector2f(size->x, size->y)});
        }
    }

    tileMapData.printTiles();
}

void PhysicsSystem::handleTileCollision(Components::AABB* entAABB, float& velocity, Components::Position* position, bool vertical, ocs::ID entityId, bool inAltWorld)
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
    tileMap.getCollidingTiles(tempAABB, start, end);

    bool aboveWindow = entities.hasComponents<Components::AboveWindow>(entityId);

    // Check the collision
    bool onPlatform = false;
    auto tileSize = tileMap.getTileSize();
    for (unsigned y = start.y; y <= end.y; ++y)
    {
        for (unsigned x = start.x; x <= end.x; ++x)
        {
            // Find which layer this object is part of
            int layer = determineLayer(inAltWorld, true, x, y);
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

                            // Add the tile ID to the set of tiles with objects on them
                            int newLayer = determineLayer(inAltWorld, aboveWindow, x, y - 1);
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
    if (vertical && entityId != ocs::ID(-1))
        es::Events::send(OnPlatformEvent{onPlatform, entityId});

    // Update the position from the new AABB
    position->x = tempAABB.left - entAABB->rect.left;
    position->y = tempAABB.top - entAABB->rect.top;
}

void PhysicsSystem::updateEdgeCases(Components::Position* position, Components::Size* size, float& velocity, ocs::ID entityId)
{
    // Ensure that entities won't move outside of the level
    if (position->x < 0)
        position->x = 0;
    if (position->y < 0)
    {
        position->y = 0;
        velocity = 0;
    }
    auto levelSize = tileMap.getPixelSize();
    sf::Vector2i newPosition(levelSize.x - size->x, levelSize.y - size->y);
    if (position->x > newPosition.x)
        position->x = newPosition.x;
    if (position->y > newPosition.y)
    {
        position->y = newPosition.y;
        es::Events::send(OnPlatformEvent{true, entityId});
    }
}

void PhysicsSystem::checkEntityCollisions()
{
    // TODO: Add "rigid" properties: Completely solid, and collidable only on the top
    // TODO: Handle collisions between collidable (rigid) components
    // TODO: Use a quad-tree or some spatial partitioning to improve performance

    // Use an O(n^2) algorithm to detect collisions between entities
    // Update the collision lists on any colliding components
    auto& aabbComponents = entities.getComponentArray<Components::AABB>();
    for (auto& aabb: aabbComponents)
    {
        aabb.collisions.clear();
        for (auto& aabb2: aabbComponents)
        {
            if (aabb.getOwnerID() != aabb2.getOwnerID())
            {
                // TODO: Dynamically build a new world when moving the window
                    // This will make things 100% accurate and much simpler (no crazy boolean logic like below)
                auto id = aabb.getOwnerID();
                auto id2 = aabb2.getOwnerID();
                auto pos = entities.getComponent<Components::Position>(id);
                auto pos2 = entities.getComponent<Components::Position>(id2);
                bool drawOnTop = entities.hasComponents<Components::DrawOnTop>(id);
                bool drawOnTop2 = entities.hasComponents<Components::DrawOnTop>(id2);
                bool inAltWorld = entities.hasComponents<Components::AltWorld>(id);
                bool inAltWorld2 = entities.hasComponents<Components::AltWorld>(id2);
                bool inWindow = magicWindow.isWithin(aabb.getGlobalBounds(pos));
                bool inWindow2 = magicWindow.isWithin(aabb2.getGlobalBounds(pos2));
                bool case1 = (inAltWorld == inWindow && inAltWorld2 == inWindow2); // "Real world"
                bool case2 = ((inAltWorld && !inWindow) && (inAltWorld2 && !inWindow2)); // "Alternate world"
                bool case3 = (!inAltWorld && drawOnTop && inWindow && inAltWorld2 && inWindow2); // An object on top, like the player
                bool case4 = (!inAltWorld2 && drawOnTop2 && inWindow2 && inAltWorld && inWindow); // Another object on top
                bool case5 = (!inAltWorld && drawOnTop && inWindow && !inAltWorld2 && drawOnTop2 && inWindow2); // Both objects on top and in window
                if (case1 || case2 || case3 || case4 || case5)
                {
                    if (aabb.getGlobalBounds(pos).intersects(aabb2.getGlobalBounds(pos2)))
                        aabb.collisions.push_back(aabb2.getOwnerID());
                }
                //if (id == 0 && id2 == 1)
                //    std::cout << "Player cases: " << case1 << ", " << case2 << ", " << case3 << ", " << case4 << ", " << case5 << ", IDs: " << id << ", " << id2 << "\n";
            }
        }
    }
}

void PhysicsSystem::checkTileCollisions()
{
    // Generate lists of tile coordinates colliding with AABB components
    // TODO: This may only be useful for the player, so make an optional flag or component to enable it
    for (auto& aabb: entities.getComponentArray<Components::AABB>())
    {
        aabb.tileCollisions.clear();
        auto position = entities.getComponent<Components::Position>(aabb.getOwnerID());
        if (position)
        {
            sf::Vector2u start, end;
            auto globalBounds = aabb.getGlobalBounds(position);
            tileMap.getCollidingTiles(globalBounds, start, end);
            bool inWindow = magicWindow.isWithin(globalBounds);
            for (unsigned y = start.y; y <= end.y; ++y)
            {
                for (unsigned x = start.x; x <= end.x; ++x)
                {
                    // Figure out which layer the tile is in
                    int layer = (inWindow && magicWindow.isWithin(tileMap.getCenterPoint(x, y)));
                    int tileId = tileMapData.getId(layer, x, y);

                    // Add the tile ID to the collision list
                    if (tileMapData(tileId).logicalId > 0)
                        aabb.tileCollisions.push_back(tileId);
                }
            }
        }
    }
}

int PhysicsSystem::determineLayer(bool inAltWorld, bool aboveWindow, unsigned x, unsigned y) const
{
    return (inAltWorld || (aboveWindow && magicWindow.isWithin(tileMap.getCenterPoint(x, y))));
}
