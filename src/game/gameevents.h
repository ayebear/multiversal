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

struct ActionKeyEvent
{
    //sf::Vector2i position;
    ocs::ID entityId;
};

struct LoadNextLevelEvent {};
struct ReloadLevelEvent {};

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

// Toggles everything connected to a switch
struct SwitchOutputEvent
{
    // Object names being switched
    std::vector<std::string> objectNames;
};

struct MovingEvent
{
    ocs::ID entityId;
    bool state;
};

struct GameFinishedEvent {};

// These events are used by the level editor
struct TileSelectionEvent
{
    int tileId;
};

struct ObjectSelectionEvent
{
    ocs::ID objectId;
    // TODO: Determine if the object name will be easier to use
};

// Used for updating the input system's view
struct ViewEvent
{
    sf::View view;
};

// Sent from the level editor so the game will load this level in memory
struct TestModeEvent
{
    // Contains an entire level as a string
    std::string level;
};

#endif
