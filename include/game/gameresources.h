// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMERESOURCES_H
#define GAMERESOURCES_H

#include <SFML/Graphics.hpp>
#include "configfile.h"
#include "nage/audio/musicplayer.h"
#include "nage/audio/soundplayer.h"
#include "gamesavehandler.h"

/*
This class contains the game specific resources:
    Window (with video mode and size)
    Game configuration file
    Music player
    Sound player
All of the states are given access to this.
*/
class GameResources: sf::NonCopyable
{
    public:
        GameResources(const std::string& windowTitle);
        ~GameResources();

        sf::VideoMode vidMode; // Window size, color settings, etc.
        sf::Vector2u windowSize; // The current window size
        sf::RenderWindow window; // The main window
        cfg::File config; // The main configuration file
        ng::MusicPlayer music; // The music player
        //ng::SoundPlayer sound; // The sound effects player
        GameSaveHandler gameSave; // To store current level

    private:
        void createWindow(const std::string& windowTitle, unsigned windowWidth, unsigned windowHeight, bool fullscreen, bool vsync, bool autoResolution);

        static const sf::Vector2u defaultResolution;
        static const sf::Vector2u minResolution;
        static const cfg::File::ConfigMap defaultOptions;
};

#endif
