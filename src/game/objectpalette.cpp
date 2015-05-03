// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "objectpalette.h"
#include "componentstrings.h"
#include "es/entityprototypeloader.h"
#include "components.h"
#include "lasersystem.h"
#include "spritesystem.h"
#include "physicssystem.h"

ocs::ID checkCollision(ocs::ObjectManager& objects, const sf::Vector2f& mousePos)
{
    for (auto& spriteComp: objects.getComponentArray<Components::Sprite>())
    {
        auto bounds = spriteComp.sprite.getGlobalBounds();
        if (bounds.contains(mousePos))
            return spriteComp.getOwnerID();
    }
    return ocs::invalidID;
}

void ObjectPalette::load(cfg::File::Section& section, Level& level)
{
    bindComponentStrings(objects);
    if (es::loadPrototypes(objects, "data/config/objects.cfg"))
    {
        level.loadObjects(section, objects, objectNames);
        LaserSystem::updateRotations(objects);
        SpriteSystem::updateRotations(objects);
    }
    else
        std::cerr << "ERROR: Could not load object prototypes.\n";
}

ocs::ID ObjectPalette::copyObject(ocs::ID sourceObject, ocs::ObjectManager& destObjects)
{
    // Serialize object from source object manager
    auto comps = objects.serializeObject(sourceObject);

    // Create a new object
    auto id = destObjects.createObject();

    for (auto& comp: comps)
    {
        auto found = comp.find(' ');
        if (found != std::string::npos && found > 0)
        {
            // Split component name and data
            auto compName = comp.substr(0, found);
            auto compData = comp.substr(found + 1);

            // Deserialize each component in destination object manager
            destObjects.updateComponentFromString(id, compName, compData);
        }
    }

    return id;
}

void ObjectPalette::draw(sf::RenderTarget& target)
{
    for (auto& spriteComp: objects.getComponentArray<Components::Sprite>())
        target.draw(spriteComp.sprite);
}
