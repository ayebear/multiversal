// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "windowfocus.h"

bool WindowFocus::focus = true;

bool WindowFocus::hasFocus()
{
    return focus;
}

void WindowFocus::setFocus(bool state)
{
    focus = state;
}
