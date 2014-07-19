// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "entity.h"

class TileMap;
class MagicWindow;

/*
This class handles applying the velocity to all of the entities' positions.
It also detects and handles collision.
*/
class PhysicsSystem
{
    public:
        PhysicsSystem(EntityList& entities, TileMap& tiles, MagicWindow& magicWindow);
        void update(float deltaTime);

    private:
        void applyGravity();
        void stepY();
        void handleYCollision();
        void stepX();
        void handleXCollision();
        void findTilesToCheck();

        EntityList& entities;
        TileMap& tiles;
        MagicWindow& magicWindow;
        Entity* ent;
        float dt;

        // Area of tiles to check
        sf::Vector2u tileSize;
        sf::Vector2i start;
        sf::Vector2i end;

};

#endif
