#ifndef CARRYSYSTEM_H
#define CARRYSYSTEM_H

#include "OCS/Objects.hpp"

class MagicWindow;

/*
This class handles entities that can "carry" other entities.
TODO: Make this more generic to have an "attacher" that overrides positions.
*/
class CarrySystem
{
    public:
        CarrySystem(ocs::ObjectManager& entities, MagicWindow& magicwindow);
        void update();

    private:
        ocs::ObjectManager& entities;
        MagicWindow& magicwindow;
};

#endif