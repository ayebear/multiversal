// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameobjects.h"
#include "statemanager.h"
#include "gamestate.h"

int main()
{
    GameObjects objects("Puzzle Game v0.0.6 Dev");
    StateManager states;
    states.addState("Game", new GameState(objects));
    states.startLoop("Game");
    return 0;
}
