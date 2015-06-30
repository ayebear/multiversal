// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "spritesystem.h"

#include <SFML/Graphics.hpp>
#include "components.h"
#include "es/world.h"

SpriteSystem::SpriteSystem(es::World& world):
    world(world)
{
}

void SpriteSystem::update(float dt)
{
    // Update sprites
    for (auto ent: world.query<Sprite>())
    {
        auto& sprite = ent.get<Sprite>()->sprite;
        setPosition(ent, sprite);
        setRotation(ent, sprite);
    }

    // Update animated sprites
    for (auto ent: world.query<AnimSprite>())
    {
        auto& sprite = ent.get<AnimSprite>()->sprite;
        setPosition(ent, sprite);
        setRotation(ent, sprite);

        // Update animated sprite
        sprite.update(dt);
    }
}

void SpriteSystem::updateRotations(es::World& world)
{
    for (auto ent: world.query<Sprite>())
    {
        auto& sprite = ent.get<Sprite>()->sprite;
        if (ent.has<Rotation>())
        {
            auto bounds = sprite.getGlobalBounds();
            sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        }
        setRotation(ent, sprite);
    }
}

void SpriteSystem::setPosition(es::Entity& ent, sf::Transformable& sprite)
{
    // Update position
    auto position = ent.get<Position>();
    if (position)
        sprite.setPosition(position->x, position->y);
}

void SpriteSystem::setRotation(es::Entity& ent, sf::Transformable& sprite)
{
    // Update rotation
    auto rotation = ent.get<Rotation>();
    if (rotation)
        sprite.setRotation(rotation->angle);
}
