// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H

#include "components.h"
#include "OCS/Objects.hpp"
#include "system.h"

class TileMap;

/*
This system handles player-related actions.
TODO: Make this more generic somehow.
    Maybe have different components that determine if something can jump, move, etc.
    Basically, get rid of the PlayerState component.
    Then, make different systems for handling these actions:
        JumpSystem
        MovementSystem
        etc.
*/
class PlayerSystem: public es::System
{
    public:
        PlayerSystem(ocs::ObjectManager& entities, TileMap& tileMap);
        void update(float dt);
        void handleJumps(Components::PlayerState& playerState);
        void handleMovement(Components::PlayerState& playerState);
        void handlePosition(Components::PlayerState& playerState);
        void handleActionKey(Components::PlayerState& playerState);

    private:
        ocs::ObjectManager& entities;
        TileMap& tileMap;
};

#endif
