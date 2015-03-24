// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef ABOUTSTATE_H
#define ABOUTSTATE_H

#include "nage/states/basestate.h"
#include <SFML/Graphics.hpp>

class GameResources;

class AboutState: public BaseState
{
    public:
        AboutState(GameResources& resources);

        void handleEvents();
        void update();
        void draw();

    private:
        void setup();

        GameResources& resources;
        std::vector<sf::Text> textList;
        sf::Sprite bgSprite;
        sf::Font font;
};

#endif
