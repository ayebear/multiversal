// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "spritepositionsystem.h"

#include <SFML/Graphics.hpp>
#include "components.h"

SpritePositionSystem::SpritePositionSystem(ocs::ObjectManager& entities):
    entities(entities)
{
}

void SpritePositionSystem::update(float dt)
{
    // Update sprite positions
    for (auto& sprite: entities.getComponentArray<Components::Sprite>())
    {
        auto position = entities.getComponent<Components::Position>(sprite.getOwnerID());
        if (position)
            sprite.sprite.setPosition(position->x, position->y);
    }

    // Update animated sprite components
    for (auto& animSprite: entities.getComponentArray<Components::AnimSprite>())
    {
        auto position = entities.getComponent<Components::Position>(animSprite.getOwnerID());
        if (position)
            animSprite.sprite.setPosition(position->x, position->y);
        animSprite.sprite.update(dt);
    }
}
