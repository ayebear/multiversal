// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "levelloader.h"
#include "level.h"
#include "es/events.h"
#include "gameevents.h"
#include "gamesavehandler.h"
#include <iostream>

LevelLoader::LevelLoader(Level& level, GameSaveHandler& gameSave, const std::string& levelDir):
    level(level),
    gameSave(gameSave),
    levelDir(levelDir)
{
}

LevelLoader::Status LevelLoader::load(int levelId)
{
    // Load the current level if not specified
    if (levelId == -1)
        levelId = gameSave.getCurrentLevel();

    // Don't load more levels than the game should have
    if (levelId > GameSaveHandler::TOTAL_LEVELS)
        return Status::Finished;

    // Get data from test mode events
    for (auto& event: es::Events::get<TestModeEvent>())
    {
        levelData = event.level;
        std::cout << "\n*** IN TEST MODE ***\n\n";
    }

    // Clear any leftover events from the previous level
    es::Events::clearAll();

    auto status = Status::Error;

    if (levelData.empty())
    {
        // Build the level filename and load it
        auto filename = getLevelFilename(levelId);
        if (level.loadFromFile(filename))
        {
            gameSave.setCurrentLevel(levelId);
            status = Status::Success;
        }
    }
    else
    {
        // Load the test level from memory
        level.loadFromString(levelData);
        status = Status::Success;
    }

    return status;
}

LevelLoader::Status LevelLoader::loadNext()
{
    return load(gameSave.getCurrentLevel() + 1);
}

bool LevelLoader::update()
{
    bool loadedLeved = false;

    // Check if the next level should be loaded
    if (es::Events::exists<LoadNextLevelEvent>())
    {
        auto status = loadNext();
        if (status == Status::Finished)
            es::Events::send(GameFinishedEvent{});
        else if (status == Status::Success)
            loadedLeved = true;
        es::Events::clear<LoadNextLevelEvent>();
    }

    // Check if the current level should be reloaded
    if (es::Events::exists<ReloadLevelEvent>())
    {
        if (load() == Status::Success)
            loadedLeved = true;
        es::Events::clear<ReloadLevelEvent>();
    }

    return loadedLeved;
}

std::string LevelLoader::getLevelFilename(int levelId) const
{
    return (levelDir + std::to_string(levelId) + ".cfg");
}

void LevelLoader::clear()
{
    levelData.clear();
}
