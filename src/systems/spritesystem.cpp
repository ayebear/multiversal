// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "spritesystem.h"

#include <SFML/Graphics.hpp>
#include "components.h"

SpriteSystem::SpriteSystem(ocs::ObjectManager& entities):
    entities(entities)
{
}

void SpriteSystem::update(float dt)
{
    // Update sprites
    for (auto& sprite: entities.getComponentArray<Components::Sprite>())
    {
        // Update position
        auto position = entities.getComponent<Components::Position>(sprite.getOwnerID());
        if (position)
            sprite.sprite.setPosition(position->x, position->y);

        // Update rotation
        auto rotation = entities.getComponent<Components::Rotation>(sprite.getOwnerID());
        if (rotation)
            sprite.sprite.setRotation(rotation->angle);
    }

    // Update animated sprites
    for (auto& animSprite: entities.getComponentArray<Components::AnimSprite>())
    {
        // Update position
        auto position = entities.getComponent<Components::Position>(animSprite.getOwnerID());
        if (position)
            animSprite.sprite.setPosition(position->x, position->y);

        // Update rotation
        auto rotation = entities.getComponent<Components::Rotation>(animSprite.getOwnerID());
        if (rotation)
            animSprite.sprite.setRotation(rotation->angle);

        // Update animated sprite
        animSprite.sprite.update(dt);
    }
}
