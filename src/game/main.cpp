// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameobjects.h"
#include "statestack.h"
#include "gamestate.h"

int main()
{
    GameObjects objects("Puzzle Game v0.1.2 Dev");
    StateStack states;
    states.add<GameState>("Game", objects);
    states.start("Game");
    return 0;
}
