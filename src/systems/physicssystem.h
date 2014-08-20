// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <SFML/Graphics.hpp>
#include "components.h"
#include "OCS/Objects.hpp"

class TileMap;
class MagicWindow;

/*
This class handles applying the velocity to all of the entities' positions.
It also detects and handles collision.
*/
class PhysicsSystem
{
    public:
        PhysicsSystem(ocs::ObjectManager& entities, TileMap& tiles, MagicWindow& magicWindow);
        void update(float dt);
        void updateSpritePositions(float dt);

    private:
        void stepPositions(float dt);
        void handleTileCollision(const sf::FloatRect& entAABB, float& velocity, Components::Position* position, bool vertical, ocs::ID entityId = -1);
        void findTilesToCheck(const sf::FloatRect& entAABB);
        void updateEdgeCases(Components::Position* position, Components::Size* size, float& velocity, ocs::ID entityId);
        void checkEntityCollisions();

        // These are used for gravity and falling
        static const sf::Vector2i maxVelocity;
        static const sf::Vector2i acceleration;

        ocs::ObjectManager& entities;
        TileMap& tiles;
        MagicWindow& magicWindow;

        // Area of tiles to check
        sf::Vector2u tileSize;
        sf::Vector2i start;
        sf::Vector2i end;
};

#endif
