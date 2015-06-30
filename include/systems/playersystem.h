// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H

#include "nage/actions/actionhandler.h"
#include "es/system.h"
#include "es/world.h"
#include "components.h"

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
        PlayerSystem(es::World& world, ng::ActionHandler& actions, Level& level);
        void initialize();
        void update(float dt);

    private:
        void handleMovement();
        void handleJump();
        void handleAction();

        es::World& world;
        ng::ActionHandler& actions;
        Level& level;

        es::ID playerId;
        bool wasRight;
};

#endif
