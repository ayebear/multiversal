// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef CAMERA_H
#define CAMERA_H

#include <map>
#include <SFML/Graphics.hpp>

/*
This class manages a container of sf::View objects.
It can apply transformations to all of the views in the container, based on the specified scale amount.
This can be used for parallax scrolling and GUIs.

Example:
    Foreground (1.0 scale, moves like normal)
    Background (0.25 scale, appears to be a background)
    HUD (0 scale, stays on the screen at all times)
*/
class Camera
{
    public:
        Camera();
        void setView(const std::string& name, const sf::View& view, float scale = 1.0f);
        const sf::View& getView(const std::string& name);
        sf::View& accessView(const std::string& name);
        void setCenter(const sf::Vector2f& center);

    private:

        struct ScaledView
        {
            sf::View view;
            float scale;
            ScaledView(): scale(1.0f) {}
            ScaledView(const sf::View& view, float scale): view(view), scale(scale) {}
        };

        std::map<std::string, ScaledView> views;
};

#endif
