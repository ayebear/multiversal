// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamesavehandler.h"

const int GameSaveHandler::TOTAL_LEVELS = 30;

const cfg::File::ConfigMap GameSaveHandler::defaultOptions = {
    {"", {
        {"currentLevel", cfg::makeOption(1, 1, TOTAL_LEVELS)}
        }
    }
};

GameSaveHandler::GameSaveHandler():
    config("user/savegame.cfg", defaultOptions),
    currentLevel(config("currentLevel"))
{
}

void GameSaveHandler::setCurrentLevel(int level)
{
    currentLevel = level;
    config.writeToFile();
}

int GameSaveHandler::getCurrentLevel() const
{
    return currentLevel.toInt();
}

void GameSaveHandler::reset()
{
    setCurrentLevel(1);
}
