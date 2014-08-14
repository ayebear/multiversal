// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include <SFML/Graphics.hpp>

class Camera;

/*
This class handles updating the camera (which contains all of the views).
*/
class CameraSystem
{
    public:
        CameraSystem(Camera& camera);
        void setMapSize(const sf::Vector2u& size);
        void update();

    private:
        Camera& camera;
        sf::Vector2u mapSize;
};

#endif
