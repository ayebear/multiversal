// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/View.hpp>
#include "OCS/Misc/Config.hpp"
#include <map>
#include <vector>

struct MousePosEvent
{
    sf::Vector2f mousePos;
};

struct MouseClickedEvent
{
    sf::Mouse::Button button;
    sf::Vector2f mousePos;
};

struct CameraEvent
{
    sf::Vector2f position;
    sf::Vector2f size;
};

// TODO: Just have a generic component to store this information
struct OnPlatformEvent
{
    bool state;
    ocs::ID entityId;
};

struct ActionKeyEvent
{
    //sf::Vector2i position;
    ocs::ID entityId;
};

struct PlayerPosition
{
    sf::Vector2f position;
    sf::Vector2u tilePos;
};

struct LoadNextLevelEvent {};

struct MapSizeEvent
{
    sf::Vector2u mapSize;
};

struct GameViewEvent
{
    sf::View gameView;
};

struct SwitchEvent
{
    enum Action
    {
        On,
        Off,
        Toggle
    };

    int tileId;
    int action;
};

// TODO: Get rid of the ability to control tiles directly, only control object names
// There could be objects that in turn control groups of tiles
using SwitchMap = std::map<int, std::vector<int> >;

using SwitchObjectMap = std::map<int, std::vector<std::string> >;

// Toggles everything connected to a switch
struct SwitchOutputEvent
{
    // Logical IDs to lists of tile IDs
    SwitchMap tileIds;

    // Object names being switched
    std::vector<std::string> objectNames;
};

// Used to update information of what switches are connected to
struct SwitchMapEvent
{
    // Tile IDs of switches to lists of connected tile IDs
    SwitchMap switches;

    // Tile IDs of switches to lists of connected objects
    SwitchObjectMap switchObjects;
};

struct MovingEvent
{
    ocs::ID entityId;
    bool state;
};

struct GameFinishedEvent {};

#endif
