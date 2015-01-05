// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef STATESTACK_H
#define STATESTACK_H

#include <string>
#include <stack>
#include <map>
#include <memory>
#include "basestate.h"
#include "stateevent.h"

/*
This class handles the deallocation/starting/changing of BaseState sub-classes.
This class is also generic, but depends on having the BaseState class and StateEvent class.
*/
class StateStack
{
    public:
        StateStack();
        ~StateStack();

        // Adds a new state (forwards the constructor arguments)
        template <typename T, typename... Args>
        void add(const std::string& name, Args&&... args);

        void remove(const std::string& name); // Removes and deallocates a state from the map
        void start(const std::string& name); // The main loop that runs until a state returns an exit event

    private:
        void deallocateStates();

        void handleEvent(StateEvent& event); // Handles a StateEvent object
        void push(const std::string& name); // Adds a state onto the stack
        void pop(); // Removes the last pushed state from the stack

        std::stack<std::string> stateStack; // Represents a stack of the states
        using StatePtr = std::unique_ptr<BaseState>; // Unique pointer to a state
        std::map<std::string, StatePtr> statePtrs; // Pointers to instances of the state types, accessed by the std::string
};

template <typename T, typename... Args>
void StateStack::add(const std::string& name, Args&&... args)
{
    statePtrs[name].reset(new T(std::forward<Args>(args)...));
}

#endif
