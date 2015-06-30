// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LASERCOMPONENT_H
#define LASERCOMPONENT_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "es/component.h"

// A single instance of a laser, including the beam(s)
struct Laser: public es::Component
{
    static constexpr auto name = "Laser";

    enum DirectionCode
    {
        Up = 0,
        Right,
        Down,
        Left
    };

    struct Beam
    {
        sf::Sprite sprite;
        int layer;
    };

    // Convert direction codes to delta values
    static const int codeToDeltaX[];
    static const int codeToDeltaY[];
    static const std::map<std::string, int> directionMap;

    std::vector<Beam> beams;
    unsigned beamCount{0};
    sf::Vector2i direction;
    std::string directionStr;

    void load(const std::string& str);
    std::string save() const;
    double getAngle(const sf::Vector2i& dir) const;
};

#endif
