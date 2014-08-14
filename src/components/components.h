// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SFML/Graphics.hpp>
#include "OCS/Components.hpp"
#include "animatedsprite.h"

namespace Components
{

// Note: Using one file for now, but may split this up later.

// Might not need some of these if I can use any type for a component

struct Position: public ocs::Component<Position>
{
    //sf::Vector2f position;
    float x, y;
};

struct Velocity: public ocs::Component<Velocity>
{
    //sf::Vector2f velocity;
    float x, y;
};

struct Size: public ocs::Component<Size>
{
    //sf::Vector2u size;
    unsigned x, y;
};

// The axis-aligned bounding box, used for collisions
struct AABB: public ocs::Component<AABB>
{
    sf::FloatRect rect;
};

// Flag for if an entity should accept user input
struct Input: public ocs::Component<Input> {};

struct Sprite: public ocs::Component<Sprite>
{
    sf::Sprite sprite;
};

struct AnimSprite: public ocs::Component<AnimSprite>
{
    AnimatedSprite sprite;
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

    PlayerState(): state(OnPlatform), wasRight(false) {}
    int state;
    bool wasRight;
};

// Flag for updating the camera
// Note: Only works if the entity also has a position component
struct CameraUpdater: public ocs::Component<CameraUpdater> {};

// TODO: Maybe add a gravity component

// Various flags and settings for the properties of an entity
// May split these up into separate components, but for now this is fine.
/*
struct Flags: public ocs::Component<Flags>
{
    Flags(): acceptInput(false) {}
    bool acceptInput;
};
*/

}

#endif
