// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "entityprototypeloader.h"
#include "configfile.h"

bool EntityPrototypeLoader::load(ocs::ObjectManager& objManager, const std::string& configFilename)
{
    cfg::File config;
    bool status = config.loadFromFile(configFilename);
    if (status)
    {
        for (auto& section: config)
        {
            for (auto& option: section.second)
                objManager.addComponentToPrototypeFromString(section.first, option.first, option.second.toString());
        }
    }
    return status;
}
