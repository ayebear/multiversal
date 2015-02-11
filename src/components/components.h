// Copyright (C) 2014-2015 Eric Hebert (ayebear)
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

/*
This component automatically updates the position component of any object.
Can be used for moving platforms, clouds, etc.
*/
struct Moving: public ocs::Component<Moving>
{
    // These are the points where it will be moved to
    std::vector<sf::Vector2f> points;

    // Settings and state
    bool state; // Controlled by switch for instance
    bool isMoving; // If it should be updating position
    bool loop; // If it should continue after the last point
    unsigned currentPoint; // Index of current destination point
    float speed; // How fast it should move
    float distance; // The total distance since the last point
    sf::Vector2f velocity; // This is a movement vector calculated from the speed and points
    sf::Vector2f startPos; // Starting position of movement

    Moving():
        state(false),
        isMoving(false),
        loop(false),
        currentPoint(0),
        speed(0.0f),
        distance(0.0f)
    {
    }

    void deSerialize(const std::string& str)
    {
        std::string pointStr;
        serializer.deSerialize("% % %s", str, loop, speed, pointStr);

        // Get the array of points as strings
        std::vector<std::string> pointStrings;
        strlib::split(pointStr, "|", pointStrings, false);
        sf::Vector2f point;

        // Parse the point values and store them in memory
        std::cout << "Points:\n";
        for (auto& str: pointStrings)
        {
            auto values = strlib::split<float>(str, ",");
            if (values.size() == 2)
            {
                points.emplace_back(values[0], values[1]);
                std::cout << values[0] << ", " << values[1] << "\n";
            }
        }
        // Note: Could just do this in the level loading code and populate the array
        // from the level file.
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
