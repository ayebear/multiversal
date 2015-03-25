// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "menustate.h"
#include "gameresources.h"
#include <iostream>

MenuState::MenuState(GameResources& resources):
    resources(resources),
    menu(resources.window, "data/config/menu.cfg")
{
    menu.addItem("Play", std::bind(&MenuState::handlePlay, this));
    menu.addItem("Level Editor", std::bind(&MenuState::handleLevelEditor, this));
    menu.addItem("About", std::bind(&MenuState::handleAbout, this));
    menu.addItem("Exit", std::bind(&MenuState::handleExit, this));
}

void MenuState::onStart()
{
    resources.window.setView(resources.window.getDefaultView());
    resources.music.play("menu");
}

void MenuState::handleEvents()
{
    sf::Event event;
    while (resources.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = ng::StateEvent::Exit;
        else
            menu.handleEvent(event);
    }
}

void MenuState::update()
{
    menu.update(dt);
    resources.music.update();
}

void MenuState::draw()
{
    resources.window.clear();
    resources.window.draw(menu);
    resources.window.display();
}

void MenuState::handlePlay()
{
    std::cout << "Play pressed\n";
    stateEvent.name = "Game";
    stateEvent.command = ng::StateEvent::Push;
}

void MenuState::handleLevelEditor()
{
    stateEvent.name = "LevelEditor";
    stateEvent.command = ng::StateEvent::Push;
}

void MenuState::handleAbout()
{
    stateEvent.name = "About";
    stateEvent.command = ng::StateEvent::Push;
}

void MenuState::handleExit()
{
    stateEvent.command = ng::StateEvent::Exit;
}
