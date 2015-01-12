// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameobjects.h"
#include "statestack.h"
#include "gamestate.h"
#include "menustate.h"

int main()
{
    GameObjects objects("Puzzle Game v0.1.3 Dev");
    StateStack states;
    states.add<GameState>("Game", objects);
    states.add<MenuState>("Menu", objects);
    states.start("Menu");
    return 0;
}
