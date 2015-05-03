// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SPRITESYSTEM_H
#define SPRITESYSTEM_H

#include "es/system.h"
#include "OCS/Objects.hpp"

namespace sf { class Transformable; }

/*
This class handles updating the sprite positions from the position components.
*/
class SpriteSystem: public es::System
{
    public:
        SpriteSystem(ocs::ObjectManager& objects);
        void update(float dt);

        static void updateRotations(ocs::ObjectManager& objects);

    private:
        static void setPosition(ocs::ObjectManager& objects, sf::Transformable& sprite, ocs::ID id);
        static void setRotation(ocs::ObjectManager& objects, sf::Transformable& sprite, ocs::ID id);

        ocs::ObjectManager& objects;
};

#endif
