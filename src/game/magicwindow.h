#ifndef MAGICWINDOW_H
#define MAGICWINDOW_H

#include <SFML/Graphics.hpp>

class MagicWindow: public sf::Drawable, public sf::Transformable
{
    public:
        MagicWindow();
        void setCenter(const sf::Vector2f& center);
        void setSize(const sf::Vector2f& newSize);
        bool hasChanged() const;
        sf::RenderTexture& getTexture();
        bool isWithin(const sf::Vector2u& pos) const;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        bool changed;
        sf::Vector2f size;
        sf::Vector2f position;
        sf::RectangleShape border;
        sf::RenderTexture texture;
};

#endif
