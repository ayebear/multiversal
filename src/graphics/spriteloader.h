// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <SFML/Graphics.hpp>
#include <map>

/*
Manages resources for textures and sprites.
Resources can be accessed with a unique string key.
Resources can be loaded from a config file, or manually.
Textures with the same filename are only loaded once, and can be used in multiple sprites.

Examples:
    SpriteLoader sprites;
    sprites.loadFromConfig("sprites.cfg");
    sprites.load("SomeSprite", "some_texture.png");
    sprites.load("SimilarSprite", "some_texture.png");
    window.draw(sprites("SomeSprite"));

Config file example:
    SomeSprite = "some_texture.png"

*/
class SpriteLoader
{
    public:
        SpriteLoader();
        SpriteLoader(const std::string& configFilename);
        bool load(const std::string& name, const std::string& textureFilename);
        bool loadFromConfig(const std::string& configFilename);
        sf::Sprite& getSprite(const std::string& name);
        sf::Texture& getTexture(const std::string& textureFilename);
        sf::Sprite& operator()(const std::string& name);

    private:
        std::map<std::string, sf::Texture> textures;
        std::map<std::string, sf::Sprite> sprites;
};

#endif
