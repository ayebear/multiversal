// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "physicssystem.h"
#include "tilemap.h"
#include "magicwindow.h"
#include "components.h"
#include "broadcasts.h"
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
    Broadcasts::clear<CameraEvent>();
    Broadcasts::clear<OnPlatformEvent>();

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

            // TODO: Use AABB component

            // Update Y
            position->y += dt * velocity.y;
            sf::FloatRect entAABB = getGlobalBounds(sf::Vector2f(position->x, position->y), sf::Vector2u(size->x, size->y));
            handleCollision(entAABB, velocity.y, position->y, size->y, true, entityId);

            // Update X
            position->x += dt * velocity.x;
            entAABB = getGlobalBounds(sf::Vector2f(position->x, position->y), sf::Vector2u(size->x, size->y));
            handleCollision(entAABB, velocity.x, position->x, size->x, false);

            // Fix edge cases
            updateEdgeCases(position, size, velocity.y, entityId);

            // Send camera update event
            if (entities.hasComponents<Components::CameraUpdater>(velocity.getOwnerID()))
                Broadcasts::send(CameraEvent{sf::Vector2f(position->x, position->y),
                                             sf::Vector2f(size->x, size->y)});
        }
    }
}

void PhysicsSystem::handleCollision(const sf::FloatRect& entAABB, float& velocity, float& position, unsigned size, bool vertical, ocs::ID entityId)
{
    // Object - tile map collision
    // Need to send an event when this happens so the entity knows which tile it is colliding with
    // And it would send what kind of event: falling off platform, standing on platform, etc.

    findTilesToCheck(entAABB);

    // Check the collision
    bool collided = false;
    bool onPlatform = false;
    for (int y = start.y; y <= end.y && !collided; ++y)
    {
        for (int x = start.x; x <= end.x && !collided; ++x)
        {
            int layer = magicWindow.isWithin(tiles.getCenterPoint(x, y)) + 1;
            if (tiles(layer, x, y) >= 1)
            {
                auto tileBox = tiles.getBoundingBox(x, y);
                //printRect(tileBox);
                if (tileBox.intersects(entAABB))
                {
                    if (vertical) // Use y
                    {
                        if (velocity >= 0) // Standing on platform
                        {
                            position = y * tileSize.y - size;
                            onPlatform = true;
                        }
                        else // Hitting ceiling
                            position = (y + 1) * tileSize.y;
                    }
                    else // Use x
                    {
                        if (velocity >= 0) // Moving to the right
                            position = x * tileSize.x - size;
                        else // Moving to the left
                            position = (x + 1) * tileSize.x;
                    }
                    velocity = 0;
                    collided = true;
                    //std::cout << "Collision detected: " << x << ", " << y << "\n";
                }
            }
        }
    }
    if (vertical && entityId != ocs::ID(-1))
        Broadcasts::send(OnPlatformEvent{onPlatform, entityId});
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
        Broadcasts::send(OnPlatformEvent{true, entityId});
    }
}

sf::FloatRect PhysicsSystem::getGlobalBounds(const sf::Vector2f& position, const sf::Vector2u& size) const
{
    // TODO: Make this customizable
    // May not even need this with an AABB component...
    return sf::FloatRect(position.x, position.y, size.x, size.y);
}
