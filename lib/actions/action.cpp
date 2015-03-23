// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "action.h"
#include "strlib.h"
#include <iostream>

namespace ng
{

bool Action::windowHasFocus = true;

const std::map<std::string, sf::Keyboard::Key> Action::strToKey = {
    {"unknown", sf::Keyboard::Unknown},
    {"a", sf::Keyboard::A},
    {"b", sf::Keyboard::B},
    {"c", sf::Keyboard::C},
    {"d", sf::Keyboard::D},
    {"e", sf::Keyboard::E},
    {"f", sf::Keyboard::F},
    {"g", sf::Keyboard::G},
    {"h", sf::Keyboard::H},
    {"i", sf::Keyboard::I},
    {"j", sf::Keyboard::J},
    {"k", sf::Keyboard::K},
    {"l", sf::Keyboard::L},
    {"m", sf::Keyboard::M},
    {"n", sf::Keyboard::N},
    {"o", sf::Keyboard::O},
    {"p", sf::Keyboard::P},
    {"q", sf::Keyboard::Q},
    {"r", sf::Keyboard::R},
    {"s", sf::Keyboard::S},
    {"t", sf::Keyboard::T},
    {"u", sf::Keyboard::U},
    {"v", sf::Keyboard::V},
    {"w", sf::Keyboard::W},
    {"x", sf::Keyboard::X},
    {"y", sf::Keyboard::Y},
    {"z", sf::Keyboard::Z},
    {"num0", sf::Keyboard::Num0},
    {"num1", sf::Keyboard::Num1},
    {"num2", sf::Keyboard::Num2},
    {"num3", sf::Keyboard::Num3},
    {"num4", sf::Keyboard::Num4},
    {"num5", sf::Keyboard::Num5},
    {"num6", sf::Keyboard::Num6},
    {"num7", sf::Keyboard::Num7},
    {"num8", sf::Keyboard::Num8},
    {"num9", sf::Keyboard::Num9},
    {"escape", sf::Keyboard::Escape},
    {"lcontrol", sf::Keyboard::LControl},
    {"lshift", sf::Keyboard::LShift},
    {"lalt", sf::Keyboard::LAlt},
    {"lsystem", sf::Keyboard::LSystem},
    {"rcontrol", sf::Keyboard::RControl},
    {"rshift", sf::Keyboard::RShift},
    {"ralt", sf::Keyboard::RAlt},
    {"rsystem", sf::Keyboard::RSystem},
    {"menu", sf::Keyboard::Menu},
    {"lbracket", sf::Keyboard::LBracket},
    {"rbracket", sf::Keyboard::RBracket},
    {"semicolon", sf::Keyboard::SemiColon},
    {"comma", sf::Keyboard::Comma},
    {"period", sf::Keyboard::Period},
    {"quote", sf::Keyboard::Quote},
    {"slash", sf::Keyboard::Slash},
    {"backslash", sf::Keyboard::BackSlash},
    {"tilde", sf::Keyboard::Tilde},
    {"equal", sf::Keyboard::Equal},
    {"dash", sf::Keyboard::Dash},
    {"space", sf::Keyboard::Space},
    {"spacebar", sf::Keyboard::Space},
    {"return", sf::Keyboard::Return},
    {"enter", sf::Keyboard::Return},
    {"backspace", sf::Keyboard::BackSpace},
    {"tab", sf::Keyboard::Tab},
    {"pageup", sf::Keyboard::PageUp},
    {"pagedown", sf::Keyboard::PageDown},
    {"end", sf::Keyboard::End},
    {"home", sf::Keyboard::Home},
    {"insert", sf::Keyboard::Insert},
    {"delete", sf::Keyboard::Delete},
    {"add", sf::Keyboard::Add},
    {"subtract", sf::Keyboard::Subtract},
    {"multiply", sf::Keyboard::Multiply},
    {"divide", sf::Keyboard::Divide},
    {"left", sf::Keyboard::Left},
    {"right", sf::Keyboard::Right},
    {"up", sf::Keyboard::Up},
    {"down", sf::Keyboard::Down},
    {"numpad0", sf::Keyboard::Numpad0},
    {"numpad1", sf::Keyboard::Numpad1},
    {"numpad2", sf::Keyboard::Numpad2},
    {"numpad3", sf::Keyboard::Numpad3},
    {"numpad4", sf::Keyboard::Numpad4},
    {"numpad5", sf::Keyboard::Numpad5},
    {"numpad6", sf::Keyboard::Numpad6},
    {"numpad7", sf::Keyboard::Numpad7},
    {"numpad8", sf::Keyboard::Numpad8},
    {"numpad9", sf::Keyboard::Numpad9},
    {"f1", sf::Keyboard::F1},
    {"f2", sf::Keyboard::F2},
    {"f3", sf::Keyboard::F3},
    {"f4", sf::Keyboard::F4},
    {"f5", sf::Keyboard::F5},
    {"f6", sf::Keyboard::F6},
    {"f7", sf::Keyboard::F7},
    {"f8", sf::Keyboard::F8},
    {"f9", sf::Keyboard::F9},
    {"f10", sf::Keyboard::F10},
    {"f11", sf::Keyboard::F11},
    {"f12", sf::Keyboard::F12},
    {"f13", sf::Keyboard::F13},
    {"f14", sf::Keyboard::F14},
    {"f15", sf::Keyboard::F15},
    {"pause", sf::Keyboard::Pause}
};

Action::Action():
    type(sf::Event::KeyPressed),
    held(false)
{
}

void Action::operator=(const std::string& str)
{
    parseString(str);
}

void Action::setCallback(CallbackType c)
{
    callback = c;
}

void Action::trigger(const sf::Event& event)
{
    if (callback && checkEvent(event))
        callback();
}

bool Action::isActive() const
{
    if (windowHasFocus && held && !keys.empty())
    {
        // Get modifier key input
        bool altPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt));
        bool ctrlPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
        bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift));
        bool systemPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LSystem) || sf::Keyboard::isKeyPressed(sf::Keyboard::RSystem));

        // Return true if any key combinations are currently being pressed
        for (auto& key: keys)
        {
            if ((key.code == sf::Keyboard::Unknown ||
                sf::Keyboard::isKeyPressed(key.code)) &&
                key.alt == altPressed &&
                key.control == ctrlPressed &&
                key.shift == shiftPressed &&
                key.system == systemPressed)
                return true;
        }
    }
    return false;
}

void Action::parseString(const std::string& str)
{
    // Map the string to an SFML event
    auto actionStr = strlib::split(str, ":");
    if (!actionStr.empty())
    {
        keys.clear();
        type = sf::Event::KeyPressed;
        held = false;
        if (actionStr.size() >= 2)
        {
            // Pressed, released, or held
            auto lowerActionStr = strlib::toLower(actionStr.front());
            if (lowerActionStr == "held")
                held = true;
            else if (lowerActionStr == "pressed")
                type = sf::Event::KeyPressed;
            else if (lowerActionStr == "released")
                type = sf::Event::KeyReleased;
        }

        // Parse the list of key combinations
        auto keyCombos = strlib::split(actionStr.back(), ",");
        for (auto& keyCombo: keyCombos)
        {
            //std::cout << "Key combo: " << keyCombo << "\n";
            auto keyEvent = getKeyEvent(keyCombo);
            if (held || keyEvent.code != sf::Keyboard::Unknown)
                keys.push_back(keyEvent);
        }
    }
}

bool Action::checkEvent(const sf::Event& event) const
{
    // If the key code, event type, and all key modifiers are the same, then return true.
    if (!held)
    {
        for (auto& key: keys)
        {
            if (event.type == type &&
                event.key.code == key.code &&
                event.key.alt == key.alt &&
                event.key.control == key.control &&
                event.key.shift == key.shift &&
                event.key.system == key.system)
                return true;
        }
    }
    return false;
}

sf::Event::KeyEvent Action::getKeyEvent(const std::string& keyCombo) const
{
    // Split up something like "Control+Shift+A" and return a KeyEvent
    sf::Event::KeyEvent keyEvent{};
    keyEvent.code = sf::Keyboard::Unknown;
    auto keyNames = strlib::split(keyCombo, "+");

    // Go through all of the key names in the combination
    for (auto& name: keyNames)
    {
        auto lowerCombo = strlib::toLower(name);
        if (lowerCombo == "alt")
            keyEvent.alt = true;
        else if (lowerCombo == "control" || lowerCombo == "ctrl")
            keyEvent.control = true;
        else if (lowerCombo == "shift")
            keyEvent.shift = true;
        else if (lowerCombo == "system")
            keyEvent.system = true;
        else
        {
            // Lookup the key name
            auto found = strToKey.find(lowerCombo);
            if (found != strToKey.end() && found->second != sf::Keyboard::Unknown)
                keyEvent.code = found->second;
            else
                std::cerr << "Error: \"" << name << "\" is not a known key name.\n";
        }
    }
    return keyEvent;
}

}
