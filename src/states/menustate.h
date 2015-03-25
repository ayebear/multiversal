// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "nage/states/basestate.h"
#include "nage/gui/gamemenu.h"

class GameResources;

class MenuState: public ng::BaseState
{
    public:
        MenuState(GameResources& resources);

        void onStart();
        void handleEvents();
        void update();
        void draw();

    private:
        void handlePlay();
        void handleLevelEditor();
        void handleAbout();
        void handleExit();

        GameResources& resources;
        ng::GameMenu menu;
};

#endif
