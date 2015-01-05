// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef BASESTATE_H
#define BASESTATE_H

#include <SFML/System.hpp>
#include "stateevent.h"

/*
This is a generic base class for all types of game states.
The state manager uses this polymorphically to switch between states.
*/
class BaseState
{
    public:
        BaseState() {}
        virtual ~BaseState() {}

        const StateEvent& start(); // Runs the main loop, then returns the action object

        // These are totally optional, they don't even need to be implemented by sub classes
        virtual void onPush() {} // Gets called only when the state has been pushed onto the stack
        virtual void onPop() {} // Gets called only when the state has been popped off of the stack

        // These are the main functions of the state, which are called every loop
        virtual void handleEvents() = 0;
        virtual void update() = 0;
        virtual void draw() = 0;

    protected:
        StateEvent stateEvent;
        float dt;

    private:
        sf::Clock clock;
};

#endif
