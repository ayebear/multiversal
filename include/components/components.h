// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>
#include <vector>
#include <set>
#include <SFML/Graphics.hpp>
#include "nage/graphics/animatedsprite.h"
#include "nage/graphics/spriteloader.h"
#include "es/component.h"
#include "es/serialize.h"
#include "es/internal/id.h"

/*
The simple components of Multiversal.
More complex components can be found in separate files.
*/

struct Position: public es::Component
{
    static constexpr auto name = "Position";

    float x, y;

    Position(float x = 0, float y = 0): x{x}, y{y} {}

    void load(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string save() const
    {
        return es::pack(x, y);
    }
};

struct Velocity: public es::Component
{
    static constexpr auto name = "Velocity";

    float x, y;

    Velocity(float x = 0, float y = 0): x{x}, y{y} {}

    void load(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string save() const
    {
        return es::pack(x, y);
    }
};

struct Size: public es::Component
{
    static constexpr auto name = "Size";

    unsigned x, y;

    Size(unsigned x = 0, unsigned y = 0): x{x}, y{y} {}

    void load(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string save() const
    {
        return es::pack(x, y);
    }
};

// The axis-aligned bounding box, used for collisions
struct AABB: public es::Component
{
    static constexpr auto name = "AABB";

    sf::FloatRect rect; // The actual AABB. Note: This has the relative position of the AABB,
        // which is (0, 0) in a lot of cases. You need to add the object's position to get
        // a bounding box with its global position. The reason for this is so the AABB
        // component does not change when the object moves around.
    std::vector<es::ID> collisions; // IDs of currently colliding world
    std::vector<int> tileCollisions; // Tile IDs of currently colliding tiles (could be in alternate world)

    void load(const std::string& str)
    {
        es::unpack(str, rect.left, rect.top, rect.width, rect.height);
    }

    std::string save() const
    {
        return es::pack(rect.left, rect.top, rect.width, rect.height);
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

struct Sprite: public es::Component
{
    static constexpr auto name = "Sprite";

    sf::Sprite sprite;
    std::string filename;
    bool visible{true};

    void load(const std::string& str)
    {
        auto values = strlib::split(str, " ");
        values.resize(2);
        filename = values.front();
        visible = (values.back().empty() || strlib::strToBool(values.back()));
        ng::SpriteLoader::load(sprite, filename, true);
    }

    std::string save() const
    {
        auto str = filename;
        if (!visible)
            str += " false";
        return str;
    }
};

struct AnimSprite: public es::Component
{
    static constexpr auto name = "AnimSprite";

    ng::AnimatedSprite sprite;
    std::string filename;
    bool visible{true};

    void load(const std::string& str)
    {
        filename = str;
        sprite.loadFromConfig(filename);
    }

    std::string save() const
    {
        return filename;
    }
};

/*
Determines if an object can "jump" at a certain speed.
Triggered by the "jump" action.
*/
struct Jumpable: public es::Component
{
    static constexpr auto name = "Jumpable";

    float jumpSpeed{0.0f};

    void load(const std::string& str)
    {
        es::unpack(str, jumpSpeed);
    }

    std::string save() const
    {
        return es::pack(jumpSpeed);
    }
};

// Determines the state of a collidable object
struct ObjectState: public es::Component
{
    static constexpr auto name = "ObjectState";

    enum State
    {
        OnPlatform = 0,
        InAir
        //Climbing // Will need this if ladders are made
    };

    int state{OnPlatform};

    void load(const std::string& str)
    {
        es::unpack(str, state);
    }

    std::string save() const
    {
        return es::pack(state);
    }
};

// Allows an object to move left/right with input
struct Movable: public es::Component
{
    static constexpr auto name = "Movable";

    float velocity{0.0f};

    void load(const std::string& str)
    {
        es::unpack(str, velocity);
    }

    std::string save() const
    {
        return es::pack(velocity);
    }
};

// Gives world the ability to carry world with Carryable components
struct Carrier: public es::Component
{
    static constexpr auto name = "Carrier";

    // ID of entity being carried
    es::ID id{0};

    // Where to position the object being carried
    sf::Vector2f offset;

    // If the carriable component is currently being carried
    bool carrying{false};

    void load(const std::string& str)
    {
        es::unpack(str, offset.x, offset.y);
    }

    std::string save() const
    {
        return es::pack(offset.x, offset.y);
    }
};

// Gives world a certain amount of gravity using a vector
struct Gravity: public es::Component
{
    static constexpr auto name = "Gravity";

    sf::Vector2f acceleration;

    void load(const std::string& str)
    {
        es::unpack(str, acceleration.x, acceleration.y);
    }

    std::string save() const
    {
        return es::pack(acceleration.x, acceleration.y);
    }
};

// A simple boolean state component
struct State: public es::Component
{
    static constexpr auto name = "State";

    bool value{false};

    void load(const std::string& str)
    {
        value = strlib::strToBool(str);
    }

    std::string save() const
    {
        return strlib::toString(value);
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
struct TileGroup: public es::Component
{
    static constexpr auto name = "TileGroup";

    bool initialState{false};
    std::set<int> tileIds;

    void load(const std::string& str)
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

    std::string save() const
    {
        return es::pack(initialState, strlib::join(tileIds, " "));
    }
};

// Holds a single tile ID, which gets converted to a position
struct TilePosition: public es::Component
{
    static constexpr auto name = "TilePosition";

    int id{0}; // Unique tile ID
    int layer{0}; // Which world
    sf::Vector2u pos; // Tile coordinates

    void load(const std::string& str)
    {
        es::unpack(str, id);
    }

    std::string save() const
    {
        return es::pack(id);
    }
};

// Contains an angle, used for rotating sprites
struct Rotation: public es::Component
{
    static constexpr auto name = "Rotation";

    float angle;

    Rotation(): angle(0.0f) {}

    void load(const std::string& str)
    {
        es::unpack(str, angle);
    }

    std::string save() const
    {
        return es::pack(angle);
    }
};

// Holds a list of object names to be controlled by a switch
struct Switch: public es::Component
{
    static constexpr auto name = "Switch";

    int tileId{0};
    std::vector<std::string> objectNames;

    void load(const std::string& str)
    {
        objectNames = strlib::split(str, " ");
        if (!objectNames.empty())
        {
            tileId = strlib::fromString<int>(objectNames.front());
            objectNames.erase(objectNames.begin());
        }
    }

    std::string save() const
    {
        return es::pack(tileId, strlib::join(objectNames, " "));
    }
};

// Initializes the position of an object by name
struct InitialPosition: public es::Component
{
    static constexpr auto name = "InitialPosition";

    std::string entityName;

    void load(const std::string& str)
    {
        es::unpack(str, entityName);
    }

    std::string save() const
    {
        return entityName;
    }
};

// Stores the name of the prototype that an entity was created from
struct Prototype: public es::Component
{
    static constexpr auto name = "Prototype";

    std::string entityName;

    Prototype(const std::string& str = ""): entityName(str) {}

    void load(const std::string& str)
    {
        es::unpack(str, entityName);
    }

    std::string save() const
    {
        return entityName;
    }
};

// Component flags

// Flag for updating the camera
// Note: Only works if the entity also has a position component
struct CameraUpdater: public es::Component
{
    static constexpr auto name = "CameraUpdater";
};

// Determines if the entity exists in the alternate world
struct AltWorld: public es::Component
{
    static constexpr auto name = "AltWorld";
};

// Determines if an entity should be drawn on top of everything
struct DrawOnTop: public es::Component
{
    static constexpr auto name = "DrawOnTop";
};

// Determines if an entity can be carried by another entity with a Carrier component
struct Carryable: public es::Component
{
    static constexpr auto name = "Carryable";
};

// Determines if an entity should be shown in the window independent of which
// world it is currently in. Example: The player entity has this component.
struct AboveWindow: public es::Component
{
    static constexpr auto name = "AboveWindow";
};

// Determines if other entities can pass through this entity or not
struct Rigid: public es::Component
{
    static constexpr auto name = "Rigid";
};

// Excludes an object from being saved in a level file
struct ExcludeFromLevel: public es::Component
{
    static constexpr auto name = "ExcludeFromLevel";
};

#endif
