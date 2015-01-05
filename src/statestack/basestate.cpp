// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "basestate.h"

// This fuction is used as the main loop in all game states.
const StateEvent& BaseState::start()
{
    stateEvent.reset();
    handleEvents();
    while (stateEvent.command == StateEvent::Continue)
    {
        dt = clock.restart().asSeconds();
        update();
        draw();
        handleEvents();
    }
    return stateEvent;
}
