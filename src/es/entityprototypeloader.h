#ifndef ENTITYPROTOTYPELOADER_H
#define ENTITYPROTOTYPELOADER_H

#include "OCS/Objects/ObjectManager.hpp"

/*
This is just a slightly simplified version of the ObjectPrototypeLoader class from OCS.
It uses cfg::File formatted files instead, to keep things consistent with the rest of the project.
Sample format:

[SomeEntity]
Position = "200 200"
Velocity = "50 50"

[AnotherEntity]
SomeComponent = "parameter1 parameter2 parameter3"
AnotherComponent = ""

*/
class EntityPrototypeLoader
{
    public:
        // Loads all prototypes from a config file (returns true if successful)
        static bool load(ocs::ObjectManager& objManager, const std::string& configFilename);
};

#endif
