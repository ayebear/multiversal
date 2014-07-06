// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef PLAYER_H
#define PLAYER_H

#include "animatedsprite.h"
#include "entity.h"
#include <iostream>

class Player: public Entity
{
    public:
        Player();
        const sf::Vector2f getPosition() const;
        const sf::Vector2u& getSize() const;
        void jump();
        void handleEvents();
        void update(float dt);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        void handleEdgeCollision();

        enum State
        {
            OnPlatform = 0,
            InAir,
            Climbing
        };

        static const int jumpSpeed = -800;

        int state;
        AnimatedSprite sprite;
        bool wasRight;
};

template <class Type>
void printRect(const sf::Rect<Type> rect)
{
    std::cout << "(" << rect.left << ", " << rect.top << ", " << rect.width << ", " << rect.height << ")\n";
}

#endif
