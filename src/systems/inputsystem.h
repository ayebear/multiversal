// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include <SFML/Graphics.hpp>
#include "es/system.h"

/*
This class handles the processing of all inputs, and sends out global input events with all of the needed information.

Note: Relies on ViewEvent events to be sent

Maybe also map the input to "actions", which are sent as events, so a "player jump" action event is sent instead of
    "space pressed" or something. Then this would allow customizable hotkeys, so that other code doesn't need to
    handle any of that.
Also, have states that can be "watched" for realtime input, like sf::Keyboard::isKeyPressed or whatever.
    These states could be stored statically, so they wouldn't have to wastefully be sent as events.
        "States" class, similar to Events and Broadcasts
    If using actions, you would access the states by using the action key, like "PlayerJumped".
    These would have to be setup by some kind of configuration or code, so that it can loop through and update these.
    Benefit of this would be everything wouldn't need to access sf::Keyboard or input, it would just check the states.
*/
class InputSystem: public es::System
{
    public:
        InputSystem(sf::RenderWindow& window);
        void update(float dt);

    private:
        void proxyEvents();
        void sendMouseButtonEvents(const sf::View& view);
        void sendMousePositionEvents(const sf::View& view);

        sf::RenderWindow& window;
        sf::Vector2f currentMousePos;

        sf::View currentView;
};

#endif
