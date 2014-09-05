// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SPRITEPOSITIONSYSTEM_H
#define SPRITEPOSITIONSYSTEM_H

#include "OCS/Objects.hpp"

/*
This class handles updating the sprite positions from the position components.
*/
class SpritePositionSystem
{
    public:
        SpritePositionSystem(ocs::ObjectManager& entities);
        void update(float dt);

    private:
        ocs::ObjectManager& entities;
};

#endif
