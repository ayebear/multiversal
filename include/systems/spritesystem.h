// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SPRITESYSTEM_H
#define SPRITESYSTEM_H

#include "es/system.h"

namespace sf { class Transformable; }
namespace es
{
    class Entity;
    class World;
}

/*
This class handles updating the sprite positions from the position components.
*/
class SpriteSystem: public es::System
{
    public:
        SpriteSystem(es::World& world);
        void update(float dt);

        static void updateRotations(es::World& world);

    private:
        static void setPosition(es::Entity& ent, sf::Transformable& sprite);
        static void setRotation(es::Entity& ent, sf::Transformable& sprite);

        es::World& world;
};

#endif
