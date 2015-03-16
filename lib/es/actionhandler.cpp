// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "actionhandler.h"

namespace es
{

ActionHandler::ActionHandler()
{
}

void ActionHandler::handleEvent(const sf::Event& event)
{
    handleFocusEvent(event);

    // Iterate through all of the sections
    for (auto& section: actions)
    {
        for (auto& action: section.second)
            action.second.trigger(event);
    }
}

void ActionHandler::handleEvent(const sf::Event& event, const std::string& sectionName)
{
    handleFocusEvent(event);

    // Iterate through the specified section
    for (auto& action: actions[sectionName])
        action.second.trigger(event);
}

Action& ActionHandler::operator[](const std::string& actionName)
{
    return actions[""][actionName];
}

Action& ActionHandler::operator()(const std::string& sectionName, const std::string& actionName)
{
    return actions[sectionName][actionName];
}

void ActionHandler::loadSection(const cfg::File::Section& section, const std::string& sectionName)
{
    for (auto& option: section)
        actions[sectionName][option.first] = option.second;
}

bool ActionHandler::loadFromConfig(const std::string& filename)
{
    cfg::File config(filename);
    for (auto& section: config)
        loadSection(section.second, section.first);
    return config.getStatus();
}

void ActionHandler::handleFocusEvent(const sf::Event& event) const
{
    // Update the window focus, so realtime input can be enabled/disabled
    if (event.type == sf::Event::LostFocus)
        Action::windowHasFocus = false;
    else if (event.type == sf::Event::GainedFocus)
        Action::windowHasFocus = true;
}

}
