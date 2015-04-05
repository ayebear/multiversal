// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "OCS/Components.hpp"
#include "nage/graphics/animatedsprite.h"
#include "nage/graphics/spriteloader.h"

/*
Contains commonly used components.
*/
namespace Components
{

struct Position: public ocs::Component<Position>
{
    float x, y;

    Position() {}
    Position(float x, float y): x(x), y(y) {}

    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }

    std::string serialize()
    {
        return serializer.serialize("Position % %", x, y);
    }
};

struct Velocity: public ocs::Component<Velocity>
{
    float x, y;

    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }

    std::string serialize()
    {
        return serializer.serialize("Velocity % %", x, y);
    }
};

struct Size: public ocs::Component<Size>
{
    unsigned x, y;

    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, x, y);
    }

    std::string serialize()
    {
        return serializer.serialize("Size % %", x, y);
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

    std::string serialize()
    {
        return serializer.serialize("AABB % % % %", rect.left, rect.top, rect.width, rect.height);
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
    std::string filename;

    void deSerialize(const std::string& str)
    {
        filename = str;
        ng::SpriteLoader::load(sprite, filename, true);
    }

    std::string serialize()
    {
        return "Sprite " + filename;
    }
};

struct AnimSprite: public ocs::Component<AnimSprite>
{
    ng::AnimatedSprite sprite;
    std::string filename;

    void deSerialize(const std::string& str)
    {
        filename = str;
        sprite.loadFromConfig(filename);
    }

    std::string serialize()
    {
        return "AnimSprite " + filename;
    }
};

/*
Determines if an object can "jump" at a certain speed.
Triggered by the "jump" action.
*/
struct Jumpable: public ocs::Component<Jumpable>
{
    float jumpSpeed{0.0f};

    void deSerialize(const std::string& str)
    {
        jumpSpeed = strlib::fromString<float>(str);
    }

    std::string serialize()
    {
        return "Jumpable " + strlib::toString(jumpSpeed);
    }
};

// Determines the state of a collidable object
struct ObjectState: public ocs::Component<ObjectState>
{
    enum State
    {
        OnPlatform = 0,
        InAir
        //Climbing // Will need this if ladders are made
    };

    int state{OnPlatform};

    std::string serialize()
    {
        return "ObjectState";
    }
};

// Allows an object to move left/right with input
struct Movable: public ocs::Component<Movable>
{
    float velocity{0.0f};

    void deSerialize(const std::string& str)
    {
        velocity = strlib::fromString<float>(str);
    }

    std::string serialize()
    {
        return "Movable " + strlib::toString(velocity);
    }
};

// Gives objects the ability to carry objects with Carryable components
struct Carrier: public ocs::Component<Carrier>
{
    // ID of entity being carried
    ocs::ID id{0};

    // Where to position the object being carried
    sf::Vector2f offset;

    // If the carriable component is currently being carried
    bool carrying{false};

    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, offset.x, offset.y);
    }

    std::string serialize()
    {
        return serializer.serialize("Carrier % %", offset.x, offset.y);
    }
};

// Gives objects a certain amount of gravity using a vector
struct Gravity: public ocs::Component<Gravity>
{
    sf::Vector2f acceleration;

    void deSerialize(const std::string& str)
    {
        serializer.deSerialize("% %", str, acceleration.x, acceleration.y);
    }

    std::string serialize()
    {
        return serializer.serialize("Gravity % %", acceleration.x, acceleration.y);
    }
};

// A simple boolean state component
struct State: public ocs::Component<State>
{
    bool value{false};

    void deSerialize(const std::string& str)
    {
        value = strlib::strToBool(str);
    }

    std::string serialize()
    {
        return "State " + strlib::toString(value);
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
        bool lastValue{false};
};

// Holds a group of tile IDs (mainly used for state switching)
struct TileGroup: public ocs::Component<TileGroup>
{
    bool initialState{false};
    std::set<int> tileIds;

    void deSerialize(const std::string& str)
    {
        initialState = false;
        tileIds.clear();
        auto tiles = strlib::split<int>(str, " ");
        if (!tiles.empty())
        {
            // Use the first element as the initial state
            initialState = (tiles.front() != 0);

            // Store the rest of the values as tile IDs
            if (tiles.size() > 1)
                tileIds.insert(tiles.begin() + 1, tiles.end());
        }
    }

    std::string serialize()
    {
        std::ostringstream stream;
        stream << "TileGroup " << initialState;
        for (int id: tileIds)
            stream << " " << id;
        return stream.str();
    }
};

// Holds a single tile ID, which gets converted to a position
struct TilePosition: public ocs::Component<TilePosition>
{
    int id{0}; // Unique tile ID
    int layer{0}; // Which world
    sf::Vector2u pos; // Tile coordinates

    void deSerialize(const std::string& str)
    {
        id = strlib::fromString<int>(str);
    }

    std::string serialize()
    {
        return "TilePosition " + strlib::toString(id);
    }
};

// Contains an angle, used for rotating sprites
struct Rotation: public ocs::Component<Rotation>
{
    float angle;

    Rotation(): angle(0.0f) {}

    void deSerialize(const std::string& str)
    {
        angle = strlib::fromString<float>(str);
    }

    std::string serialize()
    {
        return "Rotation " + strlib::toString(angle);
    }
};

// Holds a list of object names to be controlled by a switch
struct Switch: public ocs::Component<Switch>
{
    int tileId{0};
    std::vector<std::string> objectNames;

    void deSerialize(const std::string& str)
    {
        objectNames = strlib::split(str, " ");
        if (!objectNames.empty())
        {
            tileId = strlib::fromString<int>(objectNames.front());
            objectNames.erase(objectNames.begin());
        }
    }

    std::string serialize()
    {
        std::ostringstream stream;
        stream << "Switch " << tileId;
        for (const auto& name: objectNames)
            stream << " " << name;
        return stream.str();
    }
};

// Component flags

// Flag for updating the camera
// Note: Only works if the entity also has a position component
struct CameraUpdater: public ocs::Component<CameraUpdater>
{
    std::string serialize() { return "CameraUpdater"; }
};

// Determines if the entity exists in the alternate world
struct AltWorld: public ocs::Component<AltWorld>
{
    std::string serialize() { return "AltWorld"; }
};

// Determines if an entity should be drawn on top of everything
struct DrawOnTop: public ocs::Component<DrawOnTop>
{
    std::string serialize() { return "DrawOnTop"; }
};

// Determines if an entity can be carried by another entity with a Carrier component
struct Carryable: public ocs::Component<Carryable>
{
    std::string serialize() { return "Carryable"; }
};

// Determines if an entity should be shown in the window independent of which
// world it is currently in. Example: The player entity has this component.
struct AboveWindow: public ocs::Component<AboveWindow>
{
    std::string serialize() { return "AboveWindow"; }
};

// Determines if other entities can pass through this entity or not
struct Rigid: public ocs::Component<Rigid>
{
    std::string serialize() { return "Rigid"; }
};

}

#endif
