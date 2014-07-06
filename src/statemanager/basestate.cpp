// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "basestate.h"

// This fuction is used as the main loop in all game states.
const StateEvent& BaseState::start(const StateArgs& args)
{
    stateEvent.reset(); // Must reset the event in case the state still existed in memory after this returned
    processArgs(args);
    onStart();
    handleEvents();
    while (stateEvent.shouldContinue())
    {
        dt = clock.restart().asSeconds();

        // TODO: Make this configurable
        // Cap at 15 fps
        //if (dt > 0.25f)
            //dt = 0.25;

        update();
        draw();
        handleEvents(); // This is last so it doesn't have to update and draw an extra frame if the state is supposed to change
    }
    return stateEvent;
}
