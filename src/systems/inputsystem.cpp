// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "inputsystem.h"
#include "events.h"
#include "gameevents.h"

InputSystem::InputSystem(sf::RenderWindow& window):
    window(window)
{
}

void InputSystem::update(float dt)
{
    proxyEvents();
    if (Events::exists<GameViewEvent>())
    {
        // Get the game's view from an event
        auto view = Events::get<GameViewEvent>().back().gameView;
        sendMouseButtonEvents(view);
        sendMousePositionEvents(view);
    }
}

void InputSystem::proxyEvents()
{
    // Proxy SFML events from window
    Events::clear<sf::Event>();
    sf::Event event;
    while (window.pollEvent(event))
        Events::send(event);
}

void InputSystem::sendMouseButtonEvents(const sf::View& view)
{
    // Check for mouse button events, and proxy mapped coordinates
    Events::clear<MouseClickedEvent>();
    for (auto& event: Events::get<sf::Event>())
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            // Map coordinates to game view coordinates
            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2f gameMousePos = window.mapPixelToCoords(mousePos, view);
            Events::send(MouseClickedEvent{event.mouseButton.button, gameMousePos});
        }
    }
}

void InputSystem::sendMousePositionEvents(const sf::View& view)
{
    // Get mouse position - if changed, then broadcast
    Events::clear<MousePosEvent>();
    auto newMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
    if (currentMousePos != newMousePos)
    {
        currentMousePos = newMousePos;
        Events::send(MousePosEvent{currentMousePos});
    }
}
