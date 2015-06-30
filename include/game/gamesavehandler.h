// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMESAVEHANDLER_H
#define GAMESAVEHANDLER_H

#include "configfile.h"

/*
This is used to keep track of the game save.
Handles loading/saving the config file.
*/
class GameSaveHandler
{
    public:
        GameSaveHandler();

        void setCurrentLevel(int level);
        int getCurrentLevel() const;
        void reset();

        static const int TOTAL_LEVELS;

    private:
        static const cfg::File::ConfigMap defaultOptions;
        cfg::File config;
        cfg::Option& currentLevel;
};

#endif
