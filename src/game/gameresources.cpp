// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gameresources.h"
#include <iostream>

const sf::Vector2u GameResources::defaultResolution(1024, 768);
const sf::Vector2u GameResources::minResolution(640, 480);

const cfg::File::ConfigMap GameResources::defaultOptions = {
    {"Window",{
        {"vsync", cfg::makeOption(true)},
        {"fullscreen", cfg::makeOption(false)},
        {"autoResolution", cfg::makeOption(true)},
        {"windowWidth", cfg::makeOption(defaultResolution.x, minResolution.x)},
        {"windowHeight", cfg::makeOption(defaultResolution.y, minResolution.y)}
        }
    }
};

GameResources::GameResources(const std::string& windowTitle):
    config("data/config/game.cfg", defaultOptions, cfg::File::Warnings | cfg::File::Errors),
    music("data/config/music.cfg")
    //sound("sounds.cfg")
{
    config.useSection("Window");
    int windowWidth = config("windowWidth").toInt();
    int windowHeight = config("windowHeight").toInt();
    bool fullscreen = config("fullscreen").toBool();
    bool autoResolution = config("autoResolution").toBool();
    bool vsync = config("vsync").toBool();
    config.useSection();

    createWindow(windowTitle, windowWidth, windowHeight, fullscreen, vsync, autoResolution);
}

GameResources::~GameResources()
{
    window.close();
}

void GameResources::createWindow(const std::string& windowTitle, unsigned windowWidth, unsigned windowHeight, bool fullscreen, bool vsync, bool autoResolution)
{
    bool windowSizeValid = (windowWidth >= minResolution.x && windowHeight >= minResolution.y);

    // If windowed mode was specified and the sizes were invalid, use the default size
    if (!fullscreen && !windowSizeValid)
    {
        windowWidth = defaultResolution.x;
        windowHeight = defaultResolution.y;
        windowSizeValid = true;
    }

    // Use the specified window width and height if they are valid
    if (windowSizeValid && !autoResolution)
        vidMode = sf::VideoMode(windowWidth, windowHeight);
    else
        vidMode = sf::VideoMode::getDesktopMode();

    std::cout << "Using resolution: " << vidMode.width << " x " << vidMode.height << " @ " << vidMode.bitsPerPixel << " bpp\n";

    // Create the window in either fullscreen or windowed mode
    window.create(vidMode, windowTitle, (fullscreen ? sf::Style::Fullscreen : sf::Style::Close));
    window.requestFocus();

    // Set the window's vertical sync setting
    window.setVerticalSyncEnabled(vsync);

    // Cache the window size so there is no need for expensive calls to window.getSize()
    windowSize.x = vidMode.width;
    windowSize.y = vidMode.height;
}
