// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SPRITESYSTEM_H
#define SPRITESYSTEM_H

#include "OCS/Objects.hpp"
#include "system.h"

/*
This class handles updating the sprite positions from the position components.
*/
class SpriteSystem: public es::System
{
    public:
        SpriteSystem(ocs::ObjectManager& objects);
        void update(float dt);

    private:
        ocs::ObjectManager& objects;
};

#endif
