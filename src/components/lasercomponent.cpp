// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "lasercomponent.h"
#include "nage/graphics/vectors.h"

namespace Components
{

const int Laser::codeToDeltaX[] = {0, 1, 0, -1};
const int Laser::codeToDeltaY[] = {-1, 0, 1, 0};
const std::map<std::string, int> Laser::directionMap = {
    {"Up", Laser::Up},
    {"Right", Laser::Right},
    {"Down", Laser::Down},
    {"Left", Laser::Left}
};

Laser::Laser():
    beamCount(0)
{
}

void Laser::deSerialize(const std::string& str)
{
    // Convert direction text to code
    directionStr = str;
    unsigned directionCode = Up;
    auto found = directionMap.find(directionStr);
    if (found != directionMap.end())
        directionCode = found->second;

    // Convert direction code to delta values
    direction.x = codeToDeltaX[directionCode];
    direction.y = codeToDeltaY[directionCode];
}

std::string Laser::serialize()
{
    return "Laser " + directionStr;
}

double Laser::getAngle(const sf::Vector2i& dir) const
{
    return ng::vectors::rotateAngle(ng::vectors::getAngle(dir), 90.0);
}

}
