// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameobjects.h"
#include "statestack.h"
#include "menustate.h"
#include "gamestate.h"
#include "leveleditorstate.h"
#include "aboutstate.h"
#include "finalstate.h"

int main()
{
    GameObjects objects("Puzzle Game v0.1.7 Dev");
    StateStack states;
    states.add<MenuState>("Menu", objects);
    states.add<GameState>("Game", objects);
    states.add<LevelEditorState>("LevelEditor", objects);
    states.add<AboutState>("About", objects);
    states.add<FinalState>("Final", objects);
    states.start("Menu");
    return 0;
}
