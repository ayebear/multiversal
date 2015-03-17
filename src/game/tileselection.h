// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef TILESELECTION_H
#define TILESELECTION_H

#include <SFML/Graphics.hpp>

/*
A scrollable widget of tiles and objects that can be chosen from.
The chosen object is sent to the level editor.
*/
class TileSelection: public sf::Drawable
{
    public:
        TileSelection();
        void handleEvent(const sf::Event& event);
        void update(float dt);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
};

#endif
