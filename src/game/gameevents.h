// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include "OCS/Misc/Config.hpp"

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

#endif
