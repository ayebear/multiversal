// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "levelloader.h"
#include "level.h"
#include "events.h"
#include "gameevents.h"

const int LevelLoader::TOTAL_LEVELS = 1;

const cfg::File::ConfigMap LevelLoader::defaultSaveGameOptions = {
    {"", {
        {"currentLevel", cfg::makeOption(1, 1, TOTAL_LEVELS)}
        }
    }
};

LevelLoader::LevelLoader(Level& level, const std::string& levelDir):
    level(level),
    levelDir(levelDir),
    currentLevel(1),
    saveGameConfig("user/savegame.cfg", defaultSaveGameOptions, cfg::File::Autosave)
{
    if (saveGameConfig)
        currentLevel = saveGameConfig("currentLevel").toInt();
}

LevelLoader::Status LevelLoader::load(int levelId)
{
    // Load the current level if not specified
    if (levelId == -1)
        levelId = currentLevel;

    // Don't load more levels than the game should have
    if (levelId > TOTAL_LEVELS)
        return Status::Finished;

    // Get data from test mode events
    for (auto& event: es::Events::get<TestModeEvent>())
    {
        levelData = event.level;
        std::cout << "\n\n*** IN TEST MODE ***\n\n\n";
    }

    // Clear any leftover events from the previous level
    es::Events::clearAll();

    if (levelData.empty())
    {
        // Build the level filename and load it
        auto filename = getLevelFilename(levelId);
        if (level.loadFromFile(filename))
        {
            updateCurrentLevel(levelId);
            return Status::Success;
        }
    }
    else
    {
        // Load the test level from memory
        level.loadFromString(levelData);
        return Status::Success;
    }

    return Status::Error;
}

LevelLoader::Status LevelLoader::loadNext()
{
    return load(currentLevel + 1);
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

void LevelLoader::updateCurrentLevel(int levelId)
{
    currentLevel = levelId;
    saveGameConfig("currentLevel") = currentLevel;
    saveGameConfig.writeToFile();
}
