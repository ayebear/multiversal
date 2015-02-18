// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "systemcontainer.h"

namespace es
{

SystemContainer::SystemContainer()
{
}

void SystemContainer::initialize()
{
    // Call initialize on all of the systems
    for (auto& s: systems)
        s->initialize();
}

void SystemContainer::update(float dt)
{
    // Call update on all of the systems
    for (auto& s: systems)
        s->update(dt);
}

}
