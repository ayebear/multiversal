// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef ANIMATEDSPRITE_H
#define ANIMATEDSPRITE_H

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include "configfile.h"

/*
This class handles animating a sprite with a texture of frames.
Multiple animation sets are supported, each identified by a string.
It automatically calculates the sub-rectangles to use.
    You only need to specify the tile size and number of tiles
    Each animation set should be a row
    The frames are the tiles in the row

TODO:
    Handle flips (vertical and horizontal)
        Maybe make functions called flipX and flipY
        Basically a simple transformation that handles changing the origin and stuff
    Use ranges of image sequences instead of row numbers
        This would reduce the wasted space and allow for more frames because of the texture size limit
    Store textures in a map as a resource cache
    Figure out how to handle looping
        Always loop? Only play once?
        What to show when stopped/paused?
*/
class AnimatedSprite: public sf::Drawable, public sf::Transformable
{
    public:
        // Setup
        AnimatedSprite();
        //void setTexture(sf::Texture& texture);
        bool loadTexture(const std::string& textureFilename);
        void setTileSize(const sf::Vector2u& size);
        const sf::Vector2u& getTileSize() const;
        void loadFromConfig(const std::string& filename);
        void loadFromConfig(cfg::File& config);
        sf::FloatRect getGlobalBounds() const;

        // Frames/animation
        void addAnimation(const std::string& animationName, float duration, unsigned row, unsigned tileCount, bool flipX, bool flipY);
        void play(const std::string& animationName, unsigned repeat = 0);
        void play(); // Should only be called after pause() or stop()
        void pause();
        void stop();

        // Updating
        void update(float dt);
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:

        struct Animation
        {
            float duration;
            std::vector<sf::IntRect> frames;
        };
        std::map<std::string, Animation> animations;

        sf::Vector2u tileSize;
        sf::Texture texture;
        sf::Sprite sprite;

        // For playing
        enum Status
        {
            Stopped = 0,
            Playing,
            Paused
        };
        std::string currentName;
        float totalTime;
        int status; // Playing, stopped, paused
        int currentFrame;
};

#endif
