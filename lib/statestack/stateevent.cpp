// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "stateevent.h"

void StateEvent::reset()
{
    command = Continue;
    name.clear();
}
