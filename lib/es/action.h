// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef ACTION_H
#define ACTION_H

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

namespace es
{

/*
Stores an Action for the ActionHandler class.
Contains these values:
    Callback (gets called when triggered)
    Event type (if the key is pressed/released/held)
    Key code list (which keys to check)

Please note:
    Keys in the list are checked with OR logic. This allows for alternate controls.
    Key combinations are checked with AND logic, as well as having the exact
        modifier keys pressed to trigger the action.

Setting this from a string will set the key codes and event type. Example format:
    Pressed:Ctrl+W
    Released:Shift+Alt+S,Control+System+Q
*/
class Action
{
    using CallbackType = std::function<void()>;

    public:
        Action();

        // Sets the action's key code list and event type from a string
        void operator=(const std::string& str);

        // Sets the callback to trigger
        void setCallback(CallbackType c);

        // Calls the callback if the event matches any of the key combinations
        void trigger(const sf::Event& event);

        // Checks the real-time input to see if the key combinations are active
        bool isActive() const;

        static bool windowHasFocus;

    private:
        void parseString(const std::string& str);
        bool checkEvent(const sf::Event& event) const;

        // Convert strings to keys
        sf::Event::KeyEvent getKeyEvent(const std::string& keyCombo) const;

        // Used to map string names to key codes
        static const std::map<std::string, sf::Keyboard::Key> strToKey;

        CallbackType callback;
        sf::Event::EventType type; // Pressed/released
        bool held; // Use real-time input
        std::vector<sf::Event::KeyEvent> keys;
};

}

#endif
