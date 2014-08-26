// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "physicssystem.h"
#include "tilemap.h"
#include "magicwindow.h"
#include "components.h"
#include "events.h"
#include "gameevents.h"

const sf::Vector2i PhysicsSystem::maxVelocity(400, 800);
const sf::Vector2i PhysicsSystem::acceleration(1600, 1600);

PhysicsSystem::PhysicsSystem(ocs::ObjectManager& entities, TileMap& tiles, MagicWindow& magicWindow):
    entities(entities),
    tiles(tiles),
    magicWindow(magicWindow)
{
}

void PhysicsSystem::update(float dt)
{
    stepPositions(dt);
    checkEntityCollisions();
}

void PhysicsSystem::updateSpritePositions(float dt)
{
    // Update sprite positions
    for (auto& sprite: entities.getComponentArray<Components::Sprite>())
    {
        auto position = entities.getComponent<Components::Position>(sprite.getOwnerID());
        if (position)
            sprite.sprite.setPosition(position->x, position->y);
    }

    // Update animated sprite components
    for (auto& animSprite: entities.getComponentArray<Components::AnimSprite>())
    {
        auto position = entities.getComponent<Components::Position>(animSprite.getOwnerID());
        if (position)
            animSprite.sprite.setPosition(position->x, position->y);
        animSprite.sprite.update(dt);
    }
}

void PhysicsSystem::stepPositions(float dt)
{
    Events::clear<CameraEvent>();
    Events::clear<OnPlatformEvent>();

    // Apply gravity and handle collisions
    for (auto& velocity: entities.getComponentArray<Components::Velocity>())
    {
        auto entityId = velocity.getOwnerID();
        auto position = entities.getComponent<Components::Position>(entityId);
        auto size = entities.getComponent<Components::Size>(entityId);
        if (position && size)
        {
            // Apply gravity
            velocity.y += dt * acceleration.y;
            if (velocity.y > maxVelocity.y)
                velocity.y = maxVelocity.y;

            // Get the AABB component used for collision detection/handling
            auto aabb = entities.getComponent<Components::AABB>(entityId);

            // Update Y
            position->y += dt * velocity.y;
            if (aabb)
                handleTileCollision(aabb, velocity.y, position, true, entityId);

            // Update X
            position->x += dt * velocity.x;
            if (aabb)
                handleTileCollision(aabb, velocity.x, position, false, entityId);

            // Fix edge cases
            updateEdgeCases(position, size, velocity.y, entityId);

            // Send camera update event
            if (entities.hasComponents<Components::CameraUpdater>(velocity.getOwnerID()))
                Events::send(CameraEvent{sf::Vector2f(position->x, position->y),
                                             sf::Vector2f(size->x, size->y)});
        }
    }
}

void PhysicsSystem::handleTileCollision(Components::AABB* entAABB, float& velocity, Components::Position* position, bool vertical, ocs::ID entityId)
{
    // Object - tile map collision
    // Need to send an event when this happens so the entity knows which tile it is colliding with
    // And it would send what kind of event: falling off platform, standing on platform, etc.

    // Temp computed AABB (position + AABB component)
    auto tempAABB = entAABB->getGlobalBounds(position);

    // Get the area of tiles to check collision against
    findTilesToCheck(tempAABB);

    // Check the collision
    bool collided = false;
    bool onPlatform = false;
    for (int y = start.y; y <= end.y && !collided; ++y)
    {
        for (int x = start.x; x <= end.x && !collided; ++x)
        {
            int layer = 1;
            if (magicWindow.isWithin(tiles.getCenterPoint(x, y)))
                ++layer;
            if (tiles(layer, x, y) >= 1)
            {
                auto tileBox = tiles.getBoundingBox(x, y);
                //printRect(tileBox);
                if (tileBox.intersects(tempAABB))
                {
                    if (vertical) // Use y
                    {
                        if (velocity >= 0) // Standing on platform
                        {
                            tempAABB.top = y * tileSize.y - tempAABB.height;
                            onPlatform = true;
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
                    collided = true;
                    //std::cout << "Collision detected: " << x << ", " << y << "\n";
                }
            }
        }
    }

    // Notify the entity about it being in the air or on a platform
    if (vertical && entityId != ocs::ID(-1))
        Events::send(OnPlatformEvent{onPlatform, entityId});

    // Update the position from the new AABB
    position->x = tempAABB.left - entAABB->rect.left;
    position->y = tempAABB.top - entAABB->rect.top;
}

void PhysicsSystem::findTilesToCheck(const sf::FloatRect& entAABB)
{
    // Get the area to check collision against
    tileSize = tiles.getTileSize();
    start = sf::Vector2i(entAABB.left / tileSize.x, entAABB.top / tileSize.y);
    end = sf::Vector2i((entAABB.left + entAABB.width) / tileSize.x,
                       (entAABB.top + entAABB.height) / tileSize.y);
    if (start.y < 0)
        start.y = 0;
    if (start.x < 0)
        start.x = 0;
    auto mapSize = tiles.getMapSize();
    if (end.x > (int)mapSize.x - 1)
        end.x = (int)mapSize.x - 1;
    if (end.y > (int)mapSize.y - 1)
        end.y = (int)mapSize.y - 1;
    //std::cout << "start = (" << start.x << ", " << start.y << ")\n";
    //std::cout << "end = (" << end.x << ", " << end.y << ")\n";
    //rect.setPosition(start.x * tileSize.x, start.y * tileSize.x);
    //rect.setSize(sf::Vector2f((end.x - start.x + 1) * tileSize.x, (end.y - start.y + 1) * tileSize.y));
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
    auto levelSize = tiles.getPixelSize();
    sf::Vector2i newPosition(levelSize.x - size->x, levelSize.y - size->y);
    if (position->x > newPosition.x)
        position->x = newPosition.x;
    if (position->y > newPosition.y)
    {
        position->y = newPosition.y;
        Events::send(OnPlatformEvent{true, entityId});
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
