// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "movingcomponent.h"
#include "strlib.h"

namespace Components
{

void Moving::deSerialize(const std::string& str)
{
    if (!str.empty())
    {
        std::string pointStr;
        serializer.deSerialize("% % %s", str, loop, speed, pointStr);

        // Get the array of points as strings
        auto pointStrings = strlib::split(pointStr, "|");

        // Parse the point values and store them in memory
        for (const auto& str: pointStrings)
        {
            auto values = strlib::split<float>(str, ",");
            if (values.size() == 2)
                points.emplace_back(values[0], values[1]);
        }
    }
}

std::string Moving::serialize()
{
    std::string pointStr;
    for (const auto& point: points)
        pointStr += serializer.serialize("%,%|", point.x, point.y);
    if (!pointStr.empty())
        pointStr.pop_back();
    return serializer.serialize("Moving % % '%s'", loop, speed, pointStr);
}

}
