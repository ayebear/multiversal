// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "basestate.h"
#include "gamemenu.h"

class GameObjects;

class MenuState: public BaseState
{
    public:
        MenuState(GameObjects& objects);

        void onStart();
        void handleEvents();
        void update();
        void draw();

    private:
        void handlePlay();
        void handleLevelEditor();
        void handleAbout();
        void handleExit();

        GameObjects& objects;
        GameMenu menu;
};

#endif
