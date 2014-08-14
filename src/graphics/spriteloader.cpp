// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "spriteloader.h"
#include "configfile.h"

SpriteLoader::SpriteLoader()
{
}

SpriteLoader::SpriteLoader(const std::string& configFilename)
{
    loadFromConfig(configFilename);
}

bool SpriteLoader::load(const std::string& name, const std::string& textureFilename)
{
    auto& texture = textures[textureFilename];
    bool status = texture.loadFromFile(textureFilename);
    if (status)
        sprites[name].setTexture(texture, true);
    return status;
}

bool SpriteLoader::loadFromConfig(const std::string& configFilename)
{
    bool status = true;
    cfg::File config(configFilename);
    for (auto& option: config.getSection())
        status = load(option.first, option.second.toString());
    return status;
}

sf::Sprite& SpriteLoader::getSprite(const std::string& name)
{
    return sprites[name];
}

sf::Texture& SpriteLoader::getTexture(const std::string& textureFilename)
{
    return textures[textureFilename];
}

sf::Sprite& SpriteLoader::operator()(const std::string& name)
{
    return sprites[name];
}
