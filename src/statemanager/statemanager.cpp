// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "statemanager.h"

StateManager::StateManager()
{
}

StateManager::~StateManager()
{
    deallocateStates();
}

void StateManager::remove(const std::string& name)
{
    statePtrs.erase(name);
}

void StateManager::start(const std::string& name)
{
    StateEvent event(StateEvent::Push, name);
    while (event.command != StateEvent::Exit)
        handleEvent(event);
}

void StateManager::deallocateStates()
{
    // Call the remaining onPop functions
    while (!stateStack.empty())
    {
        statePtrs[stateStack.top()]->onPop();
        stateStack.pop();
    }
    // Remove and delete the allocated states
    statePtrs.clear();
}

void StateManager::handleEvent(StateEvent& event)
{
    // Take action based on the event
    if (event.command == StateEvent::Push)
        push(event.name);
    else if (event.command == StateEvent::Pop)
        pop();

    // Run the current state (on the top of the stack)
    if (!stateStack.empty())
    {
        auto& state = statePtrs[stateStack.top()];
        if (state)
            event = state->start();
    }
    else // If the stack is empty
        event.command = StateEvent::Exit; // Exit the game
}

void StateManager::push(const std::string& name)
{
    auto found = statePtrs.find(name);
    if (found != statePtrs.end() && found->second)
    {
        stateStack.push(name);
        found->second->onPush();
    }
    // If this fails due to a bad type, the current state will just start again.
}

void StateManager::pop()
{
    if (!stateStack.empty())
    {
        auto& state = statePtrs[stateStack.top()];
        if (state)
            state->onPop();
        stateStack.pop();
    }
}
