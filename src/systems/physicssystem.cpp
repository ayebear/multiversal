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

const sf::Vector2i PhysicsSystem::maxVelocity(1600, 3200);
const sf::Vector2i PhysicsSystem::gravityConstant(640, 640);

PhysicsSystem::PhysicsSystem(ocs::ObjectManager& objects, TileMapData& tileMapData, ng::TileMap& tileMap, MagicWindow& magicWindow, Level& level):
    objects(objects),
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
    for (auto& velocity: objects.getComponentArray<Components::Velocity>())
    {
        auto entityId = velocity.getOwnerID();
        auto position = objects.getComponent<Components::Position>(entityId);
        auto size = objects.getComponent<Components::Size>(entityId);
        if (position && size)
        {
            // Apply gravity from the gravity component (if it exists)
            auto gravity = objects.getComponent<Components::Gravity>(entityId);
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
            auto aabb = objects.getComponent<Components::AABB>(entityId);

            // See if the entity is in the alternate world!
            bool altWorld = inAltWorld(objects, entityId);

            // Update Y
            position->y += dt * velocity.y;
            if (aabb)
                handleTileCollision(aabb, velocity.y, position, true, entityId, altWorld);

            // Update X
            position->x += dt * velocity.x;
            if (aabb)
                handleTileCollision(aabb, velocity.x, position, false, entityId, altWorld);

            // Fix edge cases
            updateEdgeCases(position, size, velocity.y, entityId);

            // Send camera update event
            if (objects.hasComponents<Components::CameraUpdater>(velocity.getOwnerID()))
                es::Events::send(CameraEvent{sf::Vector2f(position->x, position->y),
                                             sf::Vector2f(size->x, size->y)});
        }
    }
}

void PhysicsSystem::handleTileCollision(Components::AABB* entAABB, float& velocity, Components::Position* position, bool vertical, ocs::ID entityId, bool altWorld)
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

    bool aboveWindow = objects.hasComponents<Components::AboveWindow>(entityId);

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

                            // Add the tile ID to the set of tiles with objects on them
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
    if (vertical && entityId != ocs::ID(-1))
        updateOnPlatformState(entityId, onPlatform ? Components::ObjectState::OnPlatform : Components::ObjectState::InAir);

    // Update the position from the new AABB
    position->x = tempAABB.left - entAABB->rect.left;
    position->y = tempAABB.top - entAABB->rect.top;
}

void PhysicsSystem::updateEdgeCases(Components::Position* position, Components::Size* size, float& velocity, ocs::ID entityId)
{
    auto levelSize = tileMap.getPixelSize();
    sf::Vector2i newPosition(levelSize.x - size->x, levelSize.y - size->y);

    // Ensure that entities won't move outside of the level
    ng::clampLT(position->x, 0);
    if (ng::clampLT(position->y, 0))
        velocity = 0;
    ng::clampGT(position->x, newPosition.x);
    if (ng::clampGT(position->y, newPosition.y))
        updateOnPlatformState(entityId, Components::ObjectState::OnPlatform);
}

void PhysicsSystem::checkEntityCollisions()
{
    // TODO: Add "rigid" properties: Completely solid, and collidable only on the top
    // TODO: Handle collisions between collidable (rigid) components
    // TODO: Use a quad-tree or some spatial partitioning to improve performance

    // Use an O(n^2) algorithm to detect collisions between entities
    // Update the collision lists on any colliding components
    auto& aabbComponents = objects.getComponentArray<Components::AABB>();
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
                auto pos = objects.getComponent<Components::Position>(id);
                auto pos2 = objects.getComponent<Components::Position>(id2);
                bool drawOnTop = objects.hasComponents<Components::DrawOnTop>(id);
                bool drawOnTop2 = objects.hasComponents<Components::DrawOnTop>(id2);
                bool altWorld = inAltWorld(objects, id);
                bool altWorld2 = inAltWorld(objects, id2);
                bool inWindow = magicWindow.isWithin(aabb.getGlobalBounds(pos));
                bool inWindow2 = magicWindow.isWithin(aabb2.getGlobalBounds(pos2));
                bool case1 = (altWorld == inWindow && altWorld2 == inWindow2); // "Real world"
                bool case2 = ((altWorld && !inWindow) && (altWorld2 && !inWindow2)); // "Alternate world"
                bool case3 = (!altWorld && drawOnTop && inWindow && altWorld2 && inWindow2); // An object on top, like the player
                bool case4 = (!altWorld2 && drawOnTop2 && inWindow2 && altWorld && inWindow); // Another object on top
                bool case5 = (!altWorld && drawOnTop && inWindow && !altWorld2 && drawOnTop2 && inWindow2); // Both objects on top and in window
                if (case1 || case2 || case3 || case4 || case5)
                {
                    if (aabb.getGlobalBounds(pos).intersects(aabb2.getGlobalBounds(pos2)))
                        aabb.collisions.push_back(aabb2.getOwnerID());
                }
            }
        }
    }
}

void PhysicsSystem::checkTileCollisions()
{
    // Generate lists of tile coordinates colliding with AABB components
    // TODO: This may only be useful for the player, so make an optional flag or component to enable it
    for (auto& aabb: objects.getComponentArray<Components::AABB>())
    {
        aabb.tileCollisions.clear();
        auto position = objects.getComponent<Components::Position>(aabb.getOwnerID());
        if (position)
        {
            sf::Vector2u start, end;
            auto globalBounds = aabb.getGlobalBounds(position);
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
                        aabb.tileCollisions.push_back(tileId);
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
    for (auto& initPos: objects.getComponentArray<Components::InitialPosition>())
    {
        auto objId = level.getObjectIdFromName(initPos.objectName);
        auto tilePos = objects.getComponent<Components::TilePosition>(objId);
        if (tilePos)
            tilePos->id = initPos.tileId;
    }

    // Initial positions from TilePosition components
    for (auto& tilePos: objects.getComponentArray<Components::TilePosition>())
    {
        // Update pos/layer
        tilePos.layer = tileMapData.getLayer(tilePos.id);
        tilePos.pos.x = tileMapData.getX(tilePos.id);
        tilePos.pos.y = tileMapData.getY(tilePos.id);

        auto ownerId = tilePos.getOwnerID();
        auto position = objects.getComponent<Components::Position>(ownerId);

        if (objects.hasComponents<Components::Rotation>(ownerId))
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
            auto sprite = objects.getComponent<Components::Sprite>(ownerId);
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

void PhysicsSystem::updateOnPlatformState(ocs::ID entityId, int state)
{
    auto objectState = objects.getComponent<Components::ObjectState>(entityId);
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
