// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MOVINGCOMPONENT_H
#define MOVINGCOMPONENT_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "OCS/Components.hpp"

namespace Components
{

/*
This component automatically updates the position component of any object.
Can be used for moving platforms, clouds, etc.
*/
struct Moving: public ocs::Component<Moving>
{
    // These are the points where it will be moved to
    std::vector<sf::Vector2f> points;

    // Settings and state
    bool isMoving; // If it should be updating position
    bool loop; // If it should continue after the last point
    int currentPoint; // Index of current destination point
    float speed; // How fast it should move
    float distance; // The total distance since the last point
    sf::Vector2f velocity; // This is a movement vector calculated from the speed and points
    sf::Vector2f startPos; // Starting position of movement

    Moving();
    void deSerialize(const std::string& str);
};

}

#endif
