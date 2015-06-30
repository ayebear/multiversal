// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <SFML/Graphics.hpp>
#include "components.h"
#include "es/system.h"
#include "es/world.h"

namespace ng { class TileMap; }
class TileMapData;
class MagicWindow;
class Level;

/*
This class handles applying the velocity to all of the entities' positions.
It also detects and handles collision.
*/
class PhysicsSystem: public es::System
{
    public:
        PhysicsSystem(es::World& world, TileMapData& tileMapData, ng::TileMap& tileMap, MagicWindow& magicWindow, Level& level);
        void initialize();
        void update(float dt);

    private:
        void stepPositions(float dt);
        void handleTileCollision(AABB* entAABB, float& velocity, Position* position,
                bool vertical, es::ID entityId, bool altWorld);
        void updateEdgeCases(Position* position, Size* size, float& velocity, es::ID entityId);
        void checkEntityCollisions();
        void checkTileCollisions();
        int determineLayer(bool altWorld, bool aboveWindow, unsigned x, unsigned y) const;
        void updateTilePositionComponents();
        void updateOnPlatformState(es::ID entityId, int state);
        void getCollidingTiles(const sf::FloatRect& entAABB, sf::Vector2u& start, sf::Vector2u& end);

        // These are used for gravity and falling
        static const sf::Vector2f maxVelocity;
        static const sf::Vector2f gravityConstant;

        // References
        es::World& world;
        TileMapData& tileMapData;
        ng::TileMap& tileMap;
        MagicWindow& magicWindow;
        Level& level;
};

#endif
