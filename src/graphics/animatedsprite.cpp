// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "animatedsprite.h"
#include <iostream>

AnimatedSprite::AnimatedSprite()
{
    status = Stopped;
    currentFrame = -1;
    totalTime = 0;
}

AnimatedSprite::AnimatedSprite(const std::string& configFilename):
    AnimatedSprite()
{
    loadFromConfig(configFilename);
}

bool AnimatedSprite::loadTexture(const std::string& textureFilename)
{
    bool status = texture.loadFromFile(textureFilename);
    if (status)
        sprite.setTexture(texture, true);
    return status;
}

void AnimatedSprite::setTileSize(const sf::Vector2u& size)
{
    tileSize = size;
}

const sf::Vector2u& AnimatedSprite::getTileSize() const
{
    return tileSize;
}

void AnimatedSprite::loadFromConfig(const std::string& filename)
{
    cfg::File config(filename);
    loadFromConfig(config);
}

void AnimatedSprite::loadFromConfig(cfg::File& config)
{
    loadTexture(config("texture"));
    setTileSize(sf::Vector2u(config("tileWidth").toInt(), config("tileHeight").toInt()));
    for (auto& section: config)
    {
        if (!section.first.empty())
        {
            config.useSection(section.first);
            addAnimation(section.first, config("duration").toFloat(), config("row").toInt(),
                    config("frames").toInt(), config("flipX").toBool(), config("flipY").toBool());
        }
    }
}

sf::FloatRect AnimatedSprite::getGlobalBounds() const
{
    return sf::FloatRect(getPosition().x, getPosition().y, tileSize.x, tileSize.y);
}

void AnimatedSprite::addAnimation(const std::string& animationName, float duration, unsigned row, unsigned tileCount, bool flipX, bool flipY)
{
    std::cout << "addAnimation(" << animationName << ", " << duration << ", " << row << ", " << tileCount << ", " << flipX << ", " << flipY << ")\n";
    animations[animationName].duration = duration;
    unsigned top = row * tileSize.y;
    //auto textureSize = texture.getSize(); // Will need for bounds checking
    for (unsigned count = 0; count < tileCount; ++count)
    {
        sf::IntRect frameRect(count * tileSize.x, top, tileSize.x, tileSize.y);
        if (flipX)
        {
            frameRect.left += frameRect.width;
            frameRect.width *= -1;
        }
        if (flipY)
        {
            frameRect.top += frameRect.height;
            frameRect.height *= -1;
        }
        animations[animationName].frames.push_back(frameRect);
    }
}

void AnimatedSprite::play(const std::string& animationName, unsigned repeat)
{
    if (currentName != animationName)
    {
        currentName = animationName;
        currentFrame = -1;
        totalTime = 0;
        //timer.restart();
    }
    status = Playing;
}

void AnimatedSprite::play()
{
    status = Playing;
}

void AnimatedSprite::pause()
{
    status = Paused;
}

void AnimatedSprite::stop()
{
    status = Stopped;
    currentFrame = -1;
}

void AnimatedSprite::update(float dt)
{
    if (status == Playing)
    {
        totalTime += dt;
        Animation& anim = animations[currentName];
        if (anim.duration > 0)
        {
            int frameCount = anim.frames.size();
            int nextFrame = (totalTime / anim.duration) * frameCount;
            if (nextFrame > frameCount - 1)
            {
                // Loop the animation
                nextFrame = 0;
                totalTime = 0;
            }
            if (nextFrame != currentFrame || currentFrame == -1)
            {
                currentFrame = nextFrame;
                sprite.setTextureRect(anim.frames[currentFrame]);
                //std::cout << "Setting " << currentName << " to frame " << currentFrame << ".\n";
            }
        }
    }
}

void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(sprite, states);
}
