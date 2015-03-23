// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <string>
#include "configfile.h"

class Level;

/*
Handles the loading and numbering of internal levels.
Also handles saving the current level the player is on.
    Note: May support multiple slots in the future.
Can determine if the game is completed when loading a level.
*/
class LevelLoader
{
    enum class Status
    {
        Error,
        Success,
        Finished
    };

    public:
        LevelLoader(Level& level, const std::string& levelDir);

        // Loads a level (default is to reload the current level)
        Status load(int levelId = -1);

        // Loads the next level
        Status loadNext();

        // Returns true if a new level was loaded
        bool update();

        // Returns the filename from a level ID
        std::string getLevelFilename(int levelId) const;

        // Erases the level string in memory (so it won't be in "test mode" anymore)
        void clear();

    private:

        // Saves the configuration file storing the current level
        void updateCurrentLevel(int levelId);

        static const int TOTAL_LEVELS;
        static const cfg::File::ConfigMap defaultSaveGameOptions;

        Level& level;
        std::string levelDir;
        int currentLevel;
        cfg::File saveGameConfig;
        std::string levelData;
};

#endif
