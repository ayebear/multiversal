// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef VIEWS_H
#define VIEWS_H

#include <SFML/Graphics/View.hpp>

/// Some helper functions for using views
sf::Vector2f getViewPos(const sf::Vector2f& center, const sf::Vector2f& size);
sf::Vector2f getViewPos(const sf::View& view);
sf::FloatRect getViewRect(const sf::View& view);

#endif
