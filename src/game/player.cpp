// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "player.h"
#include "tilemap.h"

Player::Player()
{
    sprite.loadFromConfig("data/config/player.cfg");
    state = OnPlatform;
    size = sprite.getTileSize();
    //rect.setFillColor(sf::Color::Transparent);
    //rect.setOutlineColor(sf::Color::Red);
    //rect.setOutlineThickness(4);
}

const sf::Vector2f Player::getPosition() const
{
    return sprite.getPosition();
}

const sf::Vector2u& Player::getSize() const
{
    return sprite.getTileSize();
}

void Player::jump()
{
    if (state == OnPlatform)
        velocity.y = jumpSpeed;
}

void Player::handleEvents()
{
    // TODO: Take in events from an input system
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        velocity.x = -maxVelocity.x;
        sprite.play("MoveLeft");
        wasRight = false;

    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        velocity.x = maxVelocity.x;
        sprite.play("MoveRight");
        wasRight = true;
    }
    else
    {
        velocity.x = 0;
        if (wasRight)
            sprite.play("StandRight");
        else
            sprite.play("StandLeft");
    }
}

void Player::update(float dt)
{
    handleEdgeCollision();
    sprite.setPosition(position);
    sprite.update(dt);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);
    //target.draw(rect, states);
}

void Player::handleEdgeCollision()
{
    if (position.x < 0)
        position.x = 0;
    if (position.y < 0)
    {
        position.y = 0;
        velocity.y = 0;
    }
    if (position.x > 2048 - size.x)
        position.x = 2048 - size.x;
    if (position.y > 768 - size.y)
    {
        position.y = 768 - size.y;
        state = OnPlatform;
    }
}
