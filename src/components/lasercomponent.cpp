#include "lasercomponent.h"

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
    beamCount(0),
    directionCode(Up)
{
}

void Laser::deSerialize(const std::string& str)
{
    std::string directionStr;
    serializer.deSerialize("%s", str, directionStr);

    // Convert direction text to code
    auto found = directionMap.find(directionStr);
    if (found == directionMap.end())
        directionCode = Up;
    else
        directionCode = found->second;

    // Convert direction code to delta values
    direction.x = codeToDeltaX[directionCode];
    direction.y = codeToDeltaY[directionCode];
}

}
