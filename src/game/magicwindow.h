// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MAGICWINDOW_H
#define MAGICWINDOW_H

#include <SFML/Graphics.hpp>

class MagicWindow: public sf::Drawable, public sf::Transformable
{
    public:
        MagicWindow();
        void update();
        void setCenter(const sf::Vector2f& center, bool force = false);
        void setSize(const sf::Vector2f& newSize);
        bool hasChanged() const;
        sf::RenderTexture& getTexture();
        bool isWithin(const sf::Vector2u& pos) const;
        bool isWithin(const sf::FloatRect& aabb) const;
        void show(bool state);
        bool isVisible() const;
        void setView(const sf::View& view, const sf::Vector2f& windowViewPos);
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        bool changed;
        bool visible;
        bool active; // If mouse input should take effect
        sf::Vector2f size;
        sf::Vector2f position;
        sf::Vector2f center;
        sf::RectangleShape border;
        sf::RectangleShape preview;
        sf::RenderTexture texture;
        sf::View textureView; // Stays the size of the texture
};

#endif
