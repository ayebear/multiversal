// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

#include <string>
#include <map>
#include "action.h"
#include "configfile.h"

namespace es
{

/*
This class handles SFML input and maps it to "actions" based on the input.
An action can trigger a callback.

Creating/updating actions is simple:
es::ActionHandler actions;
actions["action"] = "Input";

Also supports loading actions from config files:
controls.cfg:
    action = "Released:Ctrl+S"
    someAction = "Pressed:Space"

actions.loadFromConfig("controls.cfg");

After loading actions, you'll have to bind callbacks:
actions["action"].setCallback(someFunction);

For the actions to be triggered, just pass events to it in a loop:
actions.handleEvent(event);
*/
class ActionHandler
{
    using ActionMap = std::map<std::string, std::map<std::string, Action> >;

    public:
        ActionHandler();

        // Triggers any matching actions for all sections
        void handleEvent(const sf::Event& event);

        // Triggers any matching actions for a particular section
        void handleEvent(const sf::Event& event, const std::string& sectionName);

        // Returns a reference to a named action with a blank section name
        Action& operator[](const std::string& actionName);

        // Returns a reference to a named action with the section name
        Action& operator()(const std::string& sectionName, const std::string& actionName);

        // Loads a section from a configuration file
        void loadSection(const cfg::File::Section& section, const std::string& sectionName = "");

        // Loads a configuration file with actions in it
        bool loadFromConfig(const std::string& filename);

    private:
        void handleFocusEvent(const sf::Event& event) const;

        ActionMap actions;
};

}

#endif
