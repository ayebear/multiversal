// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameresources.h"
#include "statestack.h"
#include "menustate.h"
#include "gamestate.h"
#include "leveleditorstate.h"
#include "aboutstate.h"
#include "finalstate.h"

int main()
{
    GameResources resources("Multiversal v0.2.4 Dev");
    StateStack states;
    states.add<MenuState>("Menu", resources);
    states.add<GameState>("Game", resources);
    states.add<LevelEditorState>("LevelEditor", resources);
    states.add<AboutState>("About", resources);
    states.add<FinalState>("Final", resources);
    states.start("Menu");
    return 0;
}
