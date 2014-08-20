// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H

#include "components.h"
#include "OCS/Objects.hpp"

class PlayerSystem
{
    public:
        PlayerSystem(ocs::ObjectManager& entities);
        void update(float dt);
        void handleJumps(Components::PlayerState& playerState);
        void handleMovement(Components::PlayerState& playerState);
        void handlePosition(Components::PlayerState& playerState);
        void handleActionKey(Components::PlayerState& playerState);

    private:
        ocs::ObjectManager& entities;
};

#endif
