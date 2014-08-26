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
