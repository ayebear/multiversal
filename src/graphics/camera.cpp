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

void Camera::setCenter(const sf::Vector2f& center)
{
    for (auto& view: views)
    {
        // Multiply by the scale
        view.second.view.setCenter(center.x * view.second.scale, center.y * view.second.scale);
    }
}
