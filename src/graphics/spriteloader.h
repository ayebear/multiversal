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

Without an instance:
    sf::Sprite sprite;
    sf::Sprite sprite2;
    SpriteLoader::load(sprite, "texture.png");
    SpriteLoader::load(sprite2, "texture.png");
    // Will not reload texture.png

Config file example:
    SomeSprite = "some_texture.png"

*/
class SpriteLoader
{
    public:
        SpriteLoader();
        SpriteLoader(const std::string& configFilename);

        // Load a texture with a name for the sprite
        bool load(const std::string& name, const std::string& textureFilename, bool resetRect = true);

        // Load a texture into a sprite (has the benefit of not reloading the same textures)
        static bool load(sf::Sprite& sprite, const std::string& textureFilename, bool resetRect = false);

        // Loads all textures from config, and sets up sprites with those textures
        bool loadFromConfig(const std::string& configFilename);

        sf::Sprite& getSprite(const std::string& name);
        sf::Texture& getTexture(const std::string& filename);
        sf::Sprite& operator()(const std::string& name);

    private:
        static sf::Texture& loadTexture(const std::string& filename, bool& status);

        static std::map<std::string, sf::Texture> textures;
        std::map<std::string, sf::Sprite> sprites;
};

#endif
