// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <string>
#include <vector>
#include <map>
#include <SFML/Audio.hpp>
#include "configfile.h"

/*
This class handles loading and playing sets of music.
The filenames of the music files are stored in a config file.
You can also optionally shuffle the order of the music.
Once playing, it will continually loop through the entire set of music.
    It will only stop when calling stop() or pause().

To use this class, you can simply create a MusicPlayer object,
    and load a config file with a list of filenames to music files.
    Then, you can call the start() function with the name of the set
    of music to play in the config file.

Example usage:
#include "musicplayer.h"
MusicPlayer music("music.cfg");
music.start("coolSet");

Example music.cfg file:
    volume = 70
    shuffle = true
    [Music]
    coolSet = {
        "someTrack.ogg",
        "anotherTrack.ogg"
    }

TODO:
    Use a thread or future async thing or something to get rid of the update function
    Handle loading filenames from other sources besides config files
    Support audio spatialization
*/
class MusicPlayer
{
    public:
        // Setup
        MusicPlayer(const std::string& = "");
        void loadListFromConfig(const std::string&); // Loads sets of music from a config file
        void setVolume(float); // Sets the volume of all the music
        void setShuffle(bool); // Play the set in string order or randomly
        void update(); // This should be called periodically to continue playing songs

        // Controls
        void play(const std::string&); // Starts playing the specified music set
        void play(); // Starts playing again after being paused
        void skip(); // Plays the next song in the set
        void pause(); // Pauses the music (can be resumed with play())
        void stop(); // Stops playing all music
        void mute(); // Toggles volume mute

    private:
        bool play(unsigned int); // Plays a music file with the ID in the song list
        bool checkNoMusic(); // Updates the noMusic bool to efficiently check if music is playing
        void nextSongId(); // Gets the ID of the next song to play
        void checkSongId(); // Checks if the current ID is valid, otherwise it resets it
        void shuffleSongs(); // Shuffles the current song list

        enum State
        {
            Stopped = 0,
            Playing,
            Paused
        };

        using SongList = std::vector<std::string>; // A single list of music filenames

        std::map<std::string, SongList> songs; // Stores all of the song sets
        unsigned int currentSongId; // The currently playing song
        std::string currentSongSet; // The currently playing set of songs
        std::string lastSong; // The last song played (for better shuffling)
        sf::Music music; // The currently playing music object
        bool shuffle; // Whether to shuffle or play the songs in order
        bool noMusic; // For more efficient updating
        bool isMuted; // If the music is currently muted
        float volume; // Current volume (ignoring mute)
        State state; // Current state of music

        static const cfg::File::ConfigMap defaultOptions;
};

#endif
