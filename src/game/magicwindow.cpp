#include "magicwindow.h"
//#include <iostream>

MagicWindow::MagicWindow()
{
    changed = false;
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::Blue);
    border.setOutlineThickness(4);
}

void MagicWindow::setCenter(const sf::Vector2f& center)
{
    sf::Vector2f newPosition;
    newPosition.x = center.x - (size.x / 2);
    newPosition.y = center.y - (size.y / 2);
	changed = (newPosition != position);
	if (changed)
    {
        position = newPosition;
        setPosition(position);
    }
}

void MagicWindow::setSize(const sf::Vector2f& newSize)
{
	changed = (size != newSize);
	if (changed)
    {
        size = newSize;
        border.setSize(size);
        texture.create(newSize.x, newSize.y);
    }
}

bool MagicWindow::hasChanged() const
{
	return changed;
}

sf::RenderTexture& MagicWindow::getTexture()
{
    return texture;
}

bool MagicWindow::isWithin(const sf::Vector2u& pos) const
{
    return sf::Rect<unsigned>(position.x, position.y, size.x, size.y).contains(pos);
}

void MagicWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    sf::Sprite sprite(texture.getTexture());
    target.draw(sprite, states);
    target.draw(border, states);
}
