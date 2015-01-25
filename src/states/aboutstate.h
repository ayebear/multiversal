// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef ABOUTSTATE_H
#define ABOUTSTATE_H

#include "basestate.h"
#include <SFML/Graphics.hpp>

class GameObjects;

class AboutState: public BaseState
{
    public:
        AboutState(GameObjects& objects);

        void handleEvents();
        void update();
        void draw();

    private:
        void setup();

        GameObjects& objects;
        std::vector<sf::Text> textList;
        sf::Sprite bgSprite;
        sf::Font font;
};

#endif
