// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef FINALSTATE_H
#define FINALSTATE_H

#include "nage/states/basestate.h"
#include <SFML/Graphics.hpp>

class GameResources;

class FinalState: public ng::BaseState
{
    public:
        FinalState(GameResources& resources);

        void handleEvents();
        void update();
        void draw();

    private:
        GameResources& resources;
};

#endif
