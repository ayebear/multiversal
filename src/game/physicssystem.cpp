// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "physicssystem.h"

PhysicsSystem::PhysicsSystem(EntityList& entities, TileMap& tiles):
    entities(entities),
    tiles(tiles)
{
}

void PhysicsSystem::update(float deltaTime)
{
    // (Any input was just handled)

    dt = deltaTime;

    for (auto& currentEntity: entities)
    {
        ent = currentEntity.get();
        applyGravity();
        stepY();
        handleYCollision();
        stepX();
        handleXCollision();
        currentEntity->handleEvents();
        currentEntity->update(dt);
    }

    // (Drawing and stuff goes next)
}

void PhysicsSystem::applyGravity()
{
    ent->velocity.y += dt * Entity::acceleration.y;
    if (ent->velocity.y > Entity::maxVelocity.y)
        ent->velocity.y = Entity::maxVelocity.y;
}

void PhysicsSystem::stepY()
{
    ent->position.y += dt * ent->velocity.y;
}

void PhysicsSystem::handleYCollision()
{
    // Object - tile map collision
    // Need to send an event when this happens so the entity knows which tile it is colliding with
    // And it would send what kind of event: falling off platform, standing on platform, etc.

    findTilesToCheck();

    // Check the collision
    auto entAABB = ent->getGlobalBounds();
    bool collided = false;
    for (int y = start.y; y <= end.y && !collided; ++y)
    {
        for (int x = start.x; x <= end.x && !collided; ++x)
        {
            if (tiles(x, y) >= 1)
            {
                auto tileBox = tiles.getBoundingBox(x, y);
                //printRect(tileBox);
                if (tileBox.intersects(entAABB))
                {
                    if (ent->velocity.y >= 0)
                    {
                        // Standing on platform
                        ent->position.y = y * tileSize.y - ent->size.y;
                        //state = OnPlatform;
                    }
                    else
                    {
                        // Hitting ceiling
                        ent->position.y = (y + 1) * tileSize.y;
                        //state = InAir;
                    }
                    ent->velocity.y = 0;
                    collided = true;
                    //std::cout << "Collision detected: " << x << ", " << y << "\n";
                }
            }
        }
    }
}

void PhysicsSystem::stepX()
{
    ent->position.x += dt * ent->velocity.x;
}

void PhysicsSystem::handleXCollision()
{
    findTilesToCheck();

    // Check the collision
    auto entAABB = ent->getGlobalBounds();
    bool collided = false;
    for (int y = start.y; y <= end.y && !collided; ++y)
    {
        for (int x = start.x; x <= end.x && !collided; ++x)
        {
            if (tiles(x, y) >= 1)
            {
                auto tileBox = tiles.getBoundingBox(x, y);
                //printRect(tileBox);
                if (tileBox.intersects(entAABB))
                {
                    if (ent->velocity.x >= 0)
                    {
                        // Moving to the right
                        ent->position.x = x * tileSize.x - ent->size.x;
                    }
                    else
                    {
                        // Moving to the left
                        ent->position.x = (x + 1) * tileSize.x;
                    }
                    //ent->velocity.x = 0;
                    collided = true;
                    //std::cout << "Collision detected: " << x << ", " << y << "\n";
                }
            }
        }
    }
}

void PhysicsSystem::findTilesToCheck()
{
    // Get the area to check collision against
    tileSize = tiles.getTileSize();
    start = sf::Vector2i(ent->position.x / tileSize.x, ent->position.y / tileSize.y);
    end = sf::Vector2i((ent->position.x + ent->size.x) / tileSize.x,
                       (ent->position.y + ent->size.y) / tileSize.y);
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
