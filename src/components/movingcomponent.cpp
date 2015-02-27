// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "movingcomponent.h"
#include "strlib.h"

namespace Components
{

Moving::Moving():
    isMoving(false),
    loop(false),
    currentPoint(0),
    speed(0.0f),
    distance(0.0f)
{
}

void Moving::deSerialize(const std::string& str)
{
    if (!str.empty())
    {
        std::string pointStr;
        serializer.deSerialize("% % %s", str, loop, speed, pointStr);

        // Get the array of points as strings
        auto pointStrings = strlib::split(pointStr, "|");

        // Parse the point values and store them in memory
        for (auto& str: pointStrings)
        {
            auto values = strlib::split<float>(str, ",");
            if (values.size() == 2)
                points.emplace_back(values[0], values[1]);
        }
    }
}

}
