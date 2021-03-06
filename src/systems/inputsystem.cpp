// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "inputsystem.h"
#include "es/events.h"
#include "gameevents.h"

InputSystem::InputSystem(sf::RenderWindow& window):
    window(window)
{
}

void InputSystem::update(float dt)
{
    proxyEvents();

    // Update the view if any events were sent
    for (auto& event: es::Events::get<ViewEvent>())
        currentView = event.view;
    es::Events::clear<ViewEvent>();

    sendMouseButtonEvents(currentView);
    sendMousePositionEvents(currentView);
}

void InputSystem::proxyEvents()
{
    // Proxy SFML events from window
    es::Events::clear<sf::Event>();
    sf::Event event;
    while (window.pollEvent(event))
        es::Events::send(event);
}

void InputSystem::sendMouseButtonEvents(const sf::View& view)
{
    // Check for mouse button events, and proxy mapped coordinates
    es::Events::clear<MouseClickedEvent>();
    for (auto& event: es::Events::get<sf::Event>())
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            // Map coordinates to game view coordinates
            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2f gameMousePos = window.mapPixelToCoords(mousePos, view);
            es::Events::send(MouseClickedEvent{event.mouseButton.button, gameMousePos});
        }
    }
}

void InputSystem::sendMousePositionEvents(const sf::View& view)
{
    // Get mouse position - if changed, then broadcast
    es::Events::clear<MousePosEvent>();
    auto newMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
    if (currentMousePos != newMousePos)
    {
        currentMousePos = newMousePos;
        es::Events::send(MousePosEvent{currentMousePos});
    }
}
