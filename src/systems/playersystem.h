// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H

#include "components.h"
#include "OCS/Objects.hpp"
#include "system.h"
#include "nage/actions/actionhandler.h"

class Level;

/*
This system handles player-related actions:
    Movement
    Jumping
    Action key
*/
class PlayerSystem: public es::System
{
    public:
        PlayerSystem(ocs::ObjectManager& entities, ng::ActionHandler& actions, Level& level);
        void initialize();
        void update(float dt);

    private:
        void handleMovement();
        void handleJump();
        void handleAction();

        ocs::ObjectManager& entities;
        ng::ActionHandler& actions;
        Level& level;

        ocs::ID playerId;
        bool wasRight;
};

#endif
