// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "OCS/Components.hpp"
#include "animatedsprite.h"
#include "spriteloader.h"

namespace Components
{

// Note: Using one file for now, but may split this up later.

// Might not need some of these if I can use any type for a component

struct Position: public ocs::Component<Position>
{
    //sf::Vector2f position;
    float x, y;
    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }
};

struct Velocity: public ocs::Component<Velocity>
{
    //sf::Vector2f velocity;
    float x, y;
    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }
};

struct Size: public ocs::Component<Size>
{
    //sf::Vector2u size;
    unsigned x, y;
    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }
};

// The axis-aligned bounding box, used for collisions
struct AABB: public ocs::Component<AABB>
{
    sf::FloatRect rect;
    std::vector<ocs::ID> collisions; // IDs of currently colliding objects
    std::vector<sf::Vector2u> tileCollisions; // Coordinates of currently colliding tiles
    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% % % %", str, rect.left, rect.top, rect.width, rect.height);
    }
    sf::FloatRect getGlobalBounds(Position* position = nullptr)
    {
        if (position)
        {
            sf::FloatRect tempAABB(rect);
            tempAABB.left += position->x;
            tempAABB.top += position->y;
            return tempAABB;
        }
        return rect;
    }
};

struct Sprite: public ocs::Component<Sprite>
{
    sf::Sprite sprite;
    void deSerialize(const std::string& str)
    {
        std::string filename;
        serializer.deSerialize("%", str, filename);
        SpriteLoader::load(sprite, filename, true);
    }
};

struct AnimSprite: public ocs::Component<AnimSprite>
{
    AnimatedSprite sprite;
    void deSerialize(const std::string& str)
    {
        std::string filename;
        serializer.deSerialize("%", str, filename);
        sprite.loadFromConfig(filename);
    }
};

struct PlayerState: public ocs::Component<PlayerState>
{
    enum State
    {
        OnPlatform = 0,
        InAir,
        Climbing
    };

    // This could be moved elsewhere...
    static const int jumpSpeed = -800;
    // Probably a "Jumpable" component, or maybe Input

    PlayerState(): state(OnPlatform), wasRight(false) {}
    int state;
    bool wasRight;
};

struct Carrier: public ocs::Component<Carrier>
{
    // ID of entity being carried
    ocs::ID id;
    sf::Vector2f offset;
    bool carrying;

    Carrier(): id(0), carrying(false) {}

    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, offset.x, offset.y);
    }
};

// Component flags

// If an entity should accept user input
// This should be split into movable/jumpable/etc.
struct Input: public ocs::Component<Input> {};

// Flag for updating the camera
// Note: Only works if the entity also has a position component
struct CameraUpdater: public ocs::Component<CameraUpdater> {};

// Determines if the entity exists in the alternate world
struct AltWorld: public ocs::Component<AltWorld> {};

// Determines if an entity should be drawn on top of everything
struct DrawOnTop: public ocs::Component<DrawOnTop> {};

// Determines if an entity can be carried by another entity with a Carrier component
struct Carryable: public ocs::Component<Carryable> {};

// TODO: Maybe add a gravity component

}

#endif
