// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef STATEEVENT_H
#define STATEEVENT_H

#include <string>

struct StateEvent
{
    // The state manager takes action based on this
    enum Command
    {
        Continue = 0, // Continues running the current state
        Exit, // Exits the whole program
        Pop, // Goes back to the previous state
        Push // Adds a new state onto the stack
    };

    StateEvent(): command(Continue) {}
    StateEvent(const std::string& name): command(Continue), name(name) {}
    StateEvent(int command, const std::string& name): command(command), name(name) {}
    void reset();

    int command; // What to do
    std::string name; // Name of state
};

#endif
