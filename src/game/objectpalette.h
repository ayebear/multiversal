// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef OBJECTPALETTE_H
#define OBJECTPALETTE_H

#include "OCS/Objects.hpp"
#include "level.h"
#include <SFML/Graphics.hpp>

// Returns a colliding object ID from a certain point
// (Checks Position and AABB components)
ocs::ID checkCollision(ocs::ObjectManager& objects, const sf::Vector2f& mousePos);

struct ObjectPalette
{
    // Loads the prototypes and objects from a config section
    void load(cfg::File::Section& section, Level& level);

    // Copies an object and all of its components to another object manager
    ocs::ID copyObject(ocs::ID sourceObject, ocs::ObjectManager& destObjects);

    // Draws all objects (Sprite components) to a target
    void draw(sf::RenderTarget& target);

    ocs::ObjectManager objects;
    Level::ObjectNameMap objectNames;
};

#endif
