// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <SFML/Graphics.hpp>
#include "configfile.h"
#include "musicplayer.h"
#include "soundplayer.h"

/*
This class contains the game specific objects.
Handles creating the window.
All of the states are given access to this.
*/
class GameObjects: sf::NonCopyable
{
    public:
        GameObjects(const std::string&);
        ~GameObjects();

        sf::VideoMode vidMode; // Window size, color settings, etc.
        sf::Vector2u windowSize; // The current window size (if we decide to make the game resizable, this will need to be a function)
        sf::RenderWindow window; // The main window
        cfg::File config; // The main configuration file
        //MusicPlayer music; // The music player
        //SoundPlayer sound; // The sound effects player

    private:
        void createWindow(const std::string&, int, int, bool, bool); // Create a new window

        static const cfg::File::ConfigMap defaultOptions;
};

#endif
