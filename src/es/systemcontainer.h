// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SYSTEMCONTAINER_H
#define SYSTEMCONTAINER_H

#include <vector>
#include <memory>
#include "system.h"

namespace es
{

/*
This class can contain different systems, which are derived from the System base class.
It updates the systems in the order they were added.
*/
class SystemContainer
{
    public:
        SystemContainer();

        // Adds a new system. Usage:
        //     addSystem<SystemType>(constructor arguments);
        template <typename T, typename... Args>
        void add(Args&&... args);

        // Calls update() on all systems
        // Note: The order this is called is the same order the systems were added
        void update(float dt);

    private:
        using SystemPtr = std::unique_ptr<System>;
        std::vector<SystemPtr> systems;
};

template <typename T, typename... Args>
void SystemContainer::add(Args&&... args)
{
    systems.emplace_back(new T(std::forward<Args>(args)...));
}

}

#endif
