// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "spritesystem.h"

#include <SFML/Graphics.hpp>
#include "components.h"

SpriteSystem::SpriteSystem(ocs::ObjectManager& objects):
    objects(objects)
{
}

void SpriteSystem::update(float dt)
{
    // Update sprites
    for (auto& sprite: objects.getComponentArray<Components::Sprite>())
    {
        auto id = sprite.getOwnerID();

        // Update position
        auto position = objects.getComponent<Components::Position>(id);
        if (position)
            sprite.sprite.setPosition(position->x, position->y);

        // Update rotation
        auto rotation = objects.getComponent<Components::Rotation>(id);
        if (rotation)
            sprite.sprite.setRotation(rotation->angle);
    }

    // Update animated sprites
    for (auto& animSprite: objects.getComponentArray<Components::AnimSprite>())
    {
        auto id = animSprite.getOwnerID();

        // Update position
        auto position = objects.getComponent<Components::Position>(id);
        if (position)
            animSprite.sprite.setPosition(position->x, position->y);

        // Update rotation
        auto rotation = objects.getComponent<Components::Rotation>(id);
        if (rotation)
            animSprite.sprite.setRotation(rotation->angle);

        // Update animated sprite
        animSprite.sprite.update(dt);
    }
}
