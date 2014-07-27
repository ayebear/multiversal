// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "camera.h"

Camera::Camera()
{
}

void Camera::setView(const std::string& name, const sf::View& view, float scale)
{
    //views.emplace(name, view, scale);
    views[name] = ScaledView(view, scale);
}

const sf::View& Camera::getView(const std::string& name)
{
    return views[name].view;
}

sf::View& Camera::accessView(const std::string& name)
{
    return views[name].view;
}

void Camera::setCenter(const sf::Vector2f& center)
{
    for (auto& view: views)
    {
        // Multiply by the scale, and add a constant offset based on the size of the view
        auto offset = view.second.view.getSize();
        float scale = view.second.scale;
        if (scale >= 1.0f)
            offset = sf::Vector2f(0, 0);
        else
        {
            offset.x *= 0.5 * scale;
            offset.y *= 0.5 * scale;
        }
        view.second.view.setCenter(center.x * scale + offset.x, center.y * scale + offset.y);
    }
}
