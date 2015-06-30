// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LASERSYSTEM_H
#define LASERSYSTEM_H

#include <SFML/Graphics.hpp>
#include "components.h"
#include "lasercomponent.h"
#include "es/system.h"
#include "es/world.h"

namespace ng { class TileMap; }
class TileMapData;
class MagicWindow;

/*
Handles creating laser beams from lasers.
Also handles the collision and redirection of the beams.
*/
class LaserSystem: public es::System
{
    public:
        LaserSystem(es::World& world, TileMapData& tileMapData, ng::TileMap& tileMap, MagicWindow& magicWindow);
        void initialize();
        void update(float dt);

        static void updateRotations(es::World& world);

    private:
        struct PointInfo
        {
            enum class State
            {
                Stop,
                Redirect,
                Activate
            };

            PointInfo(): state(State::Stop), tileId(0) {}

            sf::Vector2f position;
            State state;
            int tileId; // The tile being hit
        };

        PointInfo findPoint();
        void addBeams(Laser& laser, TilePosition& tilePos);
        int getLayer() const;
        void changeDirection(bool state, sf::Vector2i& direction) const;

        static const char* textureFilename;

        // References
        es::World& world;
        TileMapData& tileMapData;
        ng::TileMap& tileMap;
        MagicWindow& magicWindow;

        // Game/level information
        sf::Vector2u tileSize;
        sf::Vector2u mapSize;
        unsigned beamWidth;

        // Used temporarily for making the beams
        sf::Vector2i currentPosition;
        sf::Vector2i currentDirection;
        int currentLayer;
        std::set<int> laserSensorsToDisable;
};

#endif
