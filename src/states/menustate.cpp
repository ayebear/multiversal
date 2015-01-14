// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "menustate.h"
#include "gameobjects.h"
#include <iostream>

MenuState::MenuState(GameObjects& objects):
    objects(objects),
    menu(objects.window, "data/config/menu.cfg")
{
    menu.addItem("Play", std::bind(&MenuState::handlePlay, this));
    menu.addItem("Level Editor", std::bind(&MenuState::handleLevelEditor, this));
    menu.addItem("About", std::bind(&MenuState::handleAbout, this));
    menu.addItem("Exit", std::bind(&MenuState::handleExit, this));
}

void MenuState::onStart()
{
    objects.window.setView(objects.window.getDefaultView());
}

void MenuState::handleEvents()
{
    sf::Event event;
    while (objects.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = StateEvent::Exit;
        else
            menu.handleEvent(event);
    }
}

void MenuState::update()
{
    menu.update(dt);
}

void MenuState::draw()
{
    objects.window.clear();
    objects.window.draw(menu);
    objects.window.display();
}

void MenuState::handlePlay()
{
    std::cout << "Play pressed\n";
    stateEvent.name = "Game";
    stateEvent.command = StateEvent::Push;
}

void MenuState::handleLevelEditor()
{
    stateEvent.name = "LevelEditor";
    stateEvent.command = StateEvent::Push;
}

void MenuState::handleAbout()
{
    stateEvent.name = "About";
    stateEvent.command = StateEvent::Push;
}

void MenuState::handleExit()
{
    stateEvent.command = StateEvent::Exit;
}
