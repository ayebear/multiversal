// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "entity.h"

const sf::Vector2i Entity::maxVelocity(400, 800);
const sf::Vector2i Entity::acceleration(1600, 1600);

Entity::Entity()
{
}

sf::FloatRect Entity::getGlobalBounds() const
{
    // TODO: Make this customizable
    return sf::FloatRect(position.x, position.y, size.x, size.y);
}
