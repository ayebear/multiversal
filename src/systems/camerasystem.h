// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include <SFML/Graphics.hpp>
#include "system.h"

namespace ng
{
    class Camera;
    class TileMap;
}

/*
This class handles updating the camera (which contains all of the views).
*/
class CameraSystem: public es::System
{
    public:
        CameraSystem(ng::Camera& camera, ng::TileMap& tileMap);
        void initialize();
        void update(float dt);

    private:
        ng::Camera& camera;
        ng::TileMap& tileMap;
};

#endif
