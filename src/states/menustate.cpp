// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "menustate.h"
#include "gameresources.h"
#include "nage/misc/utils.h"
#include <iostream>

MenuState::MenuState(GameResources& resources):
    resources(resources),
    menu(resources.window, "data/config/menu.cfg")
{
    continueButton = menu.addItem("Continue", ngBind(handleContinue));
    menu.addItem("New Game", ngBind(handleNewGame));
    menu.addItem("Level Editor", ngBind(handleLevelEditor));
    menu.addItem("About", ngBind(handleAbout));
    menu.addItem("Exit", ngBind(handleExit));
}

void MenuState::onStart()
{
    resources.window.setView(resources.window.getDefaultView());
    resources.music.play("menu");
    updateContinueButton();
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

void MenuState::updateContinueButton()
{
    int currentLevel = resources.gameSave.getCurrentLevel();
    bool enabled = (currentLevel > 1);
    if (enabled)
    {
        // Build button string
        std::ostringstream continueLabel;
        continueLabel << "Continue (" << currentLevel << "/" << GameSaveHandler::TOTAL_LEVELS << ")";

        // Update button label
        menu.setLabel(continueButton, continueLabel.str());
    }
    menu.setEnabled(continueButton, enabled);
}

void MenuState::handleContinue()
{
    stateEvent.name = "Game";
    stateEvent.command = ng::StateEvent::Push;
}

void MenuState::handleNewGame()
{
    stateEvent.name = "Game";
    stateEvent.command = ng::StateEvent::Push;
    resources.gameSave.reset();
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
