// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <SFML/Graphics.hpp>
#include "components.h"
#include "OCS/Objects.hpp"
#include "system.h"

class TileMapData;
class TileMap;
class MagicWindow;

/*
This class handles applying the velocity to all of the entities' positions.
It also detects and handles collision.
*/
class PhysicsSystem: public es::System
{
    public:
        PhysicsSystem(ocs::ObjectManager& entities, TileMapData& tileMapData, TileMap& tileMap, MagicWindow& magicWindow);
        void initialize();
        void update(float dt);

    private:
        void stepPositions(float dt);
        void handleTileCollision(Components::AABB* entAABB, float& velocity, Components::Position* position,
                bool vertical, ocs::ID entityId, bool inAltWorld);
        void updateEdgeCases(Components::Position* position, Components::Size* size, float& velocity, ocs::ID entityId);
        void checkEntityCollisions();
        void checkTileCollisions();
        int determineLayer(bool inAltWorld, bool aboveWindow, unsigned x, unsigned y) const;
        void updateTilePositionComponents();
        void updateOnPlatformState(ocs::ID entityId, int state);
        void getCollidingTiles(const sf::FloatRect& entAABB, sf::Vector2u& start, sf::Vector2u& end);

        // These are used for gravity and falling
        static const sf::Vector2i maxVelocity;
        static const sf::Vector2i gravityConstant;

        // References
        ocs::ObjectManager& entities;
        TileMapData& tileMapData;
        TileMap& tileMap;
        MagicWindow& magicWindow;
};

#endif
