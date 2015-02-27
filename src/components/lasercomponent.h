#ifndef LASERCOMPONENT_H
#define LASERCOMPONENT_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "OCS/Components.hpp"

namespace Components
{

// A single instance of a laser, including the beam(s)
struct Laser: public ocs::Component<Laser>
{
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
    unsigned beamCount;

    // Starts here
    sf::Vector2i direction;
    unsigned directionCode;

    Laser();
    void deSerialize(const std::string& str);
};

}

#endif
