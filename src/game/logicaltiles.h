#ifndef LOGICALTILES_H_INCLUDED
#define LOGICALTILES_H_INCLUDED

namespace Tiles
{
    enum LogicalTiles
    {
        // Surface tiles
        None = 0,
        Normal = 1,
        Glass = 2,
        Cracked = 3,
        Sticky = 4,
        Slippery = 5,

        // Special movement tiles
        Ladder = 6,
        ConveyorBelt = 7,

        // Switches
        PushButton = 8,
        ToggleSwitch = 9,
        LaserSensor = 10,

        // Other tiles
        Mirror = 12,
        ForceField = 13,

        // Level tiles
        Checkpoint = 20,
        Start = 21,
        Exit = 22
    };
}

#endif
