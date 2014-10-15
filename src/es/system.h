// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SYSTEM_H
#define SYSTEM_H

namespace es
{

/*
Base class for all systems.
Derived classes can be used with the SystemContainer class.
*/
class System
{
    public:
        System() {}
        virtual ~System() {}

        // Derived classes must implement this function
        virtual void update(float dt) = 0;
};

}

#endif
