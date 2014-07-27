#include "magicwindow.h"
#include <iostream>
#include "views.h"

MagicWindow::MagicWindow()
{
    changed = false;
    visible = false;
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
        this->center = center;
        setPosition(position);
        textureView.setCenter(center);
    }
}

void MagicWindow::setSize(const sf::Vector2f& newSize)
{
	changed = (size != newSize);
	if (changed)
    {
        size = newSize;
        border.setSize(size);
        texture.create(size.x, size.y);
        textureView.setSize(size);
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
    return (visible && sf::Rect<unsigned>(position.x, position.y, size.x, size.y).contains(pos));
}

void MagicWindow::show(bool state)
{
    visible = state;
}

bool MagicWindow::isVisible() const
{
    return visible;
}

void MagicWindow::setView(const sf::View& view, const sf::Vector2f& windowViewPos)
{
    // A regular full sized view is passed in (the size of the window)
    // Use the render texture position and size to calculate the new position
    // of the new view to use for the render texture.

    auto viewRect = getViewRect(view);
    auto textureViewRect = getViewRect(textureView);
    sf::Vector2f absolutePosition(position.x - windowViewPos.x, position.y - windowViewPos.y);
    textureViewRect.left = viewRect.left + absolutePosition.x;
    textureViewRect.top = viewRect.top + absolutePosition.y;
    textureViewRect.width = size.x;
    textureViewRect.height = size.y;
    textureView.reset(textureViewRect);
    texture.setView(textureView);
}

void MagicWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    if (visible)
    {
        sf::Sprite sprite(texture.getTexture());
        target.draw(sprite, states);
        target.draw(border, states);
    }
}
