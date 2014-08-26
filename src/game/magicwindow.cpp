// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "magicwindow.h"
#include <iostream>
#include "views.h"
#include "events.h"
#include "gameevents.h"

MagicWindow::MagicWindow()
{
    changed = false;
    visible = false;
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::Blue);
    border.setOutlineThickness(4);
    preview.setFillColor(sf::Color::Transparent);
    preview.setOutlineColor(sf::Color(128, 128, 128, 128));
    preview.setOutlineThickness(4);
}

void MagicWindow::update()
{
    // Check SFML events
    for (auto& event: Events::get<sf::Event>())
    {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            active = false;
    }
    // Check real-time mouse input
    for (auto& event: Events::get<MousePosEvent>())
        setCenter(event.mousePos);
    // Check mouse-clicked events
    for (auto& event: Events::get<MouseClickedEvent>())
    {
        if (event.button == sf::Mouse::Left)
        {
            active = true;
            visible = true;
            setCenter(event.mousePos);
        }
        else if (event.button == sf::Mouse::Right)
        {
            active = false;
            visible = false;
        }
    }
}

void MagicWindow::setCenter(const sf::Vector2f& center, bool force)
{
    if (force || active)
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
    preview.setPosition(center);
}

void MagicWindow::setSize(const sf::Vector2f& newSize)
{
    changed = (size != newSize);
    if (changed)
    {
        size = newSize;
        border.setSize(size);
        preview.setSize(size);
        preview.setOrigin(size.x / 2, size.y / 2);
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
    return (visible && sf::IntRect(position.x, position.y, size.x, size.y).contains(sf::Vector2i(pos.x, pos.y)));
}

bool MagicWindow::isWithin(const sf::FloatRect& aabb) const
{
    return (visible && sf::FloatRect(position.x, position.y, size.x, size.y).intersects(aabb));
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
    target.draw(preview);
}
