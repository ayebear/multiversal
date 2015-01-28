// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELEDITORSTATE_H
#define LEVELEDITORSTATE_H

#include "basestate.h"
#include <SFML/Graphics.hpp>

class GameObjects;

class LevelEditorState: public BaseState
{
    public:
        LevelEditorState(GameObjects& objects);

        void handleEvents();
        void update();
        void draw();

    private:
        GameObjects& objects;
};

#endif
