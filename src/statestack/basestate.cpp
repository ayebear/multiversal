// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "basestate.h"

const float BaseState::dtMax = 1.0f / 15.0f;

// This fuction is used as the main loop in all game states.
const StateEvent& BaseState::start()
{
    stateEvent.reset();
    onStart();

    // Main loop
    handleEvents();
    while (stateEvent.command == StateEvent::Continue)
    {
        // Get delta time since last frame
        dt = clock.restart().asSeconds();
        if (dt >= dtMax)
            dt = dtMax;

        // Run our state's main methods
        update();
        draw();
        handleEvents();
    }
    return stateEvent;
}
