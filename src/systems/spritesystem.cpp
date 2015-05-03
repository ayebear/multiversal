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
        setPosition(objects, sprite.sprite, id);
        setRotation(objects, sprite.sprite, id);
    }

    // Update animated sprites
    for (auto& animSprite: objects.getComponentArray<Components::AnimSprite>())
    {
        auto id = animSprite.getOwnerID();
        setPosition(objects, animSprite.sprite, id);
        setRotation(objects, animSprite.sprite, id);

        // Update animated sprite
        animSprite.sprite.update(dt);
    }
}

void SpriteSystem::updateRotations(ocs::ObjectManager& objects)
{
    for (auto& sprite: objects.getComponentArray<Components::Sprite>())
    {
        auto id = sprite.getOwnerID();
        if (objects.hasComponents<Components::Rotation>(id))
        {
            auto bounds = sprite.sprite.getGlobalBounds();
            sprite.sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        }
        setRotation(objects, sprite.sprite, id);
    }
}

void SpriteSystem::setPosition(ocs::ObjectManager& objects, sf::Transformable& sprite, ocs::ID id)
{
    // Update position
    auto position = objects.getComponent<Components::Position>(id);
    if (position)
        sprite.setPosition(position->x, position->y);
}

void SpriteSystem::setRotation(ocs::ObjectManager& objects, sf::Transformable& sprite, ocs::ID id)
{
    // Update rotation
    auto rotation = objects.getComponent<Components::Rotation>(id);
    if (rotation)
        sprite.setRotation(rotation->angle);
}
