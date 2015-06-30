// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MOVINGCOMPONENT_H
#define MOVINGCOMPONENT_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "es/component.h"

/*
This component automatically updates the position component of any object.
Can be used for moving platforms, clouds, etc.
*/
struct Moving: public es::Component
{
    static constexpr auto name = "Moving";

    // These are the points where it will be moved to
    std::vector<sf::Vector2f> points;

    // Settings
    bool loop{false}; // If it should continue after the last point
    float speed{0.0f}; // How fast it should move

    // State
    bool isMoving{false}; // If it should be updating position
    int currentPoint{0}; // Index of current destination point
    float distance{0.0f}; // The total distance since the last point
    sf::Vector2f velocity; // This is a movement vector calculated from the speed and points
    sf::Vector2f startPos; // Starting position of movement

    void load(const std::string& str);
    std::string save() const;
};

#endif
