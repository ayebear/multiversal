// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "statestack.h"
#include <iostream>

StateStack::StateStack()
{
}

StateStack::~StateStack()
{
    // Call the remaining onPop functions
    while (pop());
}

void StateStack::remove(const std::string& name)
{
    statePtrs.erase(name);
}

void StateStack::start(const std::string& name)
{
    // Push a state onto the stack and run it
    StateEvent event(StateEvent::Push, name);
    while (event.command != StateEvent::Exit)
        handleEvent(event);
}

void StateStack::handleEvent(StateEvent& event)
{
    // Take action based on the event
    if (event.command == StateEvent::Push)
        push(event.name);
    else if (event.command == StateEvent::Pop)
        pop();
    else if (event.command == StateEvent::Change)
    {
        pop();
        push(event.name);
    }

    // Run the current state (on the top of the stack)
    if (!stateStack.empty())
    {
        auto name = stateStack.top();
        auto state = getState(name);
        if (state)
        {
            std::cout << "StateStack: Running state '" << name << "'...\n";
            event = state->start();
        }
    }
    else // If the stack is empty
        event.command = StateEvent::Exit; // Exit the program
}

void StateStack::push(const std::string& name)
{
    // Only push a state when it exists
    auto state = getState(name);
    if (state)
    {
        stateStack.push(name);
        state->onPush();
        std::cout << "StateStack: Pushed state '" << name << "'.\n";
    }
    else
        std::cout << "StateStack: Error pushing state '" << name << "'.\n";
    // If this fails due to a bad type, the current state will just start again.
}

bool StateStack::pop()
{
    bool status = !stateStack.empty();
    if (status)
    {
        // Call onPop and pop the state's name off the stack
        auto name = stateStack.top();
        auto state = getState(name);
        if (state)
            state->onPop();
        stateStack.pop();
        std::cout << "StateStack: Popped state '" << name << "'.\n";
    }
    return status;
}

BaseState* StateStack::getState(const std::string& name)
{
    auto found = statePtrs.find(name);
    if (found != statePtrs.end())
        return found->second.get();
    return nullptr;
}
