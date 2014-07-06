// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

/*
This class contains the common things for a graphical entity.
Depending on how many entities will be needed besides the player, I will switch
    to using an entity component system.
*/
class Entity: public sf::Drawable
{
    public:
        Entity();
        sf::FloatRect getGlobalBounds() const;
        virtual void handleEvents() {}
        virtual void update(float dt) {}
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {}

        // These are used for gravity and falling
        static const sf::Vector2i maxVelocity;
        static const sf::Vector2i acceleration;

        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Vector2u size;

    private:
};

using EntityList = std::vector< std::unique_ptr<Entity> >;

#endif
