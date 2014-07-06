// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameobjects.h"

const cfg::File::ConfigMap GameObjects::defaultOptions = {
    {"Window",{
        {"vsync", cfg::makeOption(true)},
        {"fullscreen", cfg::makeOption(false)},
        {"windowWidth", cfg::makeOption(1024, 640)},
        {"windowHeight", cfg::makeOption(768, 480)}
        }
    }
};

GameObjects::GameObjects(const std::string& windowTitle):
    config("data/config/game.cfg", defaultOptions, cfg::File::AllFlags)
    //music("music.cfg"),
    //sound("sounds.cfg")
{
    config.useSection("Window");
    int windowWidth = config("windowWidth").toInt();
    int windowHeight = config("windowHeight").toInt();
    bool fullscreen = config("fullscreen").toBool();
    bool vsync = config("vsync").toBool();
    config.useSection();

    createWindow(windowTitle, windowWidth, windowHeight, fullscreen, vsync);
}

GameObjects::~GameObjects()
{
    window.close();
}

void GameObjects::createWindow(const std::string& windowTitle, int windowWidth, int windowHeight, bool fullscreen, bool vsync)
{
    bool windowSizeValid = (windowWidth >= 640 && windowHeight >= 480);

    // If windowed mode was specified and the sizes were invalid, use the default size
    if (!fullscreen && !windowSizeValid)
    {
        windowWidth = 1024;
        windowHeight = 768;
        windowSizeValid = true;
    }

    // Use the specified window width and height if they are valid
    if (windowSizeValid)
        vidMode = sf::VideoMode(windowWidth, windowHeight);
    else
        vidMode = sf::VideoMode::getDesktopMode();

    // Either create the window in fullscreen or windowed mode
    window.create(vidMode, windowTitle, (fullscreen ? sf::Style::Fullscreen : sf::Style::Close));

    // Set the window's vertical sync setting
    window.setVerticalSyncEnabled(vsync);

    // Cache the window size so there is no need for expensive calls to window.getSize()
    windowSize.x = vidMode.width;
    windowSize.y = vidMode.height;
}
