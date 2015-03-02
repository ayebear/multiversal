// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "OCS/Components.hpp"
#include "animatedsprite.h"
#include "spriteloader.h"

/*
Contains commonly used components.
*/
namespace Components
{

struct Position: public ocs::Component<Position>
{
    float x, y;
    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }
};

struct Velocity: public ocs::Component<Velocity>
{
    float x, y;
    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }
};

struct Size: public ocs::Component<Size>
{
    unsigned x, y;
    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }
};

// The axis-aligned bounding box, used for collisions
struct AABB: public ocs::Component<AABB>
{
    sf::FloatRect rect; // The actual AABB. Note: This has the relative position of the AABB,
        // which is (0, 0) in a lot of cases. You need to add the object's position to get
        // a bounding box with its global position. The reason for this is so the AABB
        // component does not change when the object moves around.
    std::vector<ocs::ID> collisions; // IDs of currently colliding objects
    std::vector<int> tileCollisions; // Tile IDs of currently colliding tiles (could be in alternate world)
    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% % % %", str, rect.left, rect.top, rect.width, rect.height);
    }

    // Returns a global AABB based on a position
    sf::FloatRect getGlobalBounds(Position* position = nullptr)
    {
        if (position)
        {
            // Add the position to the position of the AABB
            sf::FloatRect tempAABB(rect);
            tempAABB.left += position->x;
            tempAABB.top += position->y;
            return tempAABB;
        }

        // When the position component is not specified, the original AABB will be returned.
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

/*
TODO: Remove this component, and split it into generic components.
    This will be neccessary in the future, when objects and boxes need to know
    if they are on a platform or in the air, for example.
*/
struct PlayerState: public ocs::Component<PlayerState>
{
    enum State
    {
        OnPlatform = 0,
        InAir,
        Climbing
    };

    // This could be moved elsewhere...
    static const int jumpSpeed = -3200;
    // Probably a "Jumpable" component, or maybe Input

    PlayerState(): state(OnPlatform), wasRight(false) {}
    int state;
    bool wasRight;
};

struct Carrier: public ocs::Component<Carrier>
{
    // ID of entity being carried
    ocs::ID id;

    // Where to position the object being carried
    sf::Vector2f offset;

    // If the carriable component is currently being carried
    bool carrying;

    Carrier(): id(0), carrying(false) {}

    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, offset.x, offset.y);
    }
};

struct Gravity: public ocs::Component<Gravity>
{
    sf::Vector2f acceleration;

    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, acceleration.x, acceleration.y);
    }
};

// A simple boolean state component
struct State: public ocs::Component<State>
{
    bool value;

    State(): value(false), lastValue(false) {}

    void deSerialize(const std::string& str)
    {
        value = strlib::strToBool(str);
    }

    bool hasChanged()
    {
        if (value != lastValue)
        {
            lastValue = value;
            return true;
        }
        return false;
    }

    private:
        bool lastValue;
};

// Holds a group of tile IDs (mainly used for state switching)
struct TileGroup: public ocs::Component<TileGroup>
{
    bool initialState;
    std::vector<int> tileIds;

    TileGroup(): initialState(false) {}

    void deSerialize(const std::string& str)
    {
        initialState = false;
        tileIds = strlib::split<int>(str, " ");
        if (!tileIds.empty())
        {
            // Use the first element as the initial state
            initialState = (tileIds.front() != 0);
            tileIds.erase(tileIds.begin());
        }
    }
};

// Holds a single tile ID, which gets converted to a position
struct TilePosition: public ocs::Component<TilePosition>
{
    int id; // Unique tile ID
    int layer; // Which world
    sf::Vector2u pos; // Tile coordinates

    TilePosition(): id(0), layer(0) {}

    void deSerialize(const std::string& str)
    {
        id = fromString<int>(str);
    }
};

// Contains an angle, used for rotating sprites
struct Rotation: public ocs::Component<Rotation>
{
    float angle;

    Rotation(): angle(0.0f) {}

    void deSerialize(const std::string& str)
    {
        angle = fromString<float>(str);
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

// Determines if an entity should be shown in the window independent of which
// world it is currently in. Example: The player entity has this component.
struct AboveWindow: public ocs::Component<AboveWindow> {};

// Determines if other entities can pass through this entity or not
struct Rigid: public ocs::Component<Rigid> {};

}

#endif
