// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "inputsystem.h"
#include "broadcasts.h"
#include "gameevents.h"

InputSystem::InputSystem(sf::RenderWindow& window):
    window(window)
{
}

void InputSystem::update(const sf::View& view)
{
    proxyEvents();
    sendMouseButtonEvents(view);
    sendMousePositionEvents(view);
}

void InputSystem::proxyEvents()
{
    // Proxy SFML events from window
    Broadcasts::clear<sf::Event>();
    sf::Event event;
    while (window.pollEvent(event))
        Broadcasts::send(event);
}

void InputSystem::sendMouseButtonEvents(const sf::View& view)
{
    // Check for mouse button events, and proxy mapped coordinates
    Broadcasts::clear<MouseClickedEvent>();
    for (auto& event: Broadcasts::get<sf::Event>())
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2f gameMousePos = window.mapPixelToCoords(mousePos, view);
            Broadcasts::send(MouseClickedEvent{event.mouseButton.button, gameMousePos});
        }
    }
}

void InputSystem::sendMousePositionEvents(const sf::View& view)
{
    // Get mouse position - if changed, then broadcast
    Broadcasts::clear<MousePosEvent>();
    auto newMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
    if (currentMousePos != newMousePos)
    {
        currentMousePos = newMousePos;
        Broadcasts::send(MousePosEvent{currentMousePos});
    }
}
