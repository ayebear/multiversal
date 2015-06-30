// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "leveleditorstate.h"
#include "gameresources.h"
#include "inputsystem.h"
#include "es/events.h"

LevelEditorState::LevelEditorState(GameResources& resources):
    resources(resources)
{
}

void LevelEditorState::onStart()
{
    es::Events::clearAll();
    if (!gameInstance)
    {
        std::cout << "\nLevel editor is loading...\n\n";
        gameInstance = std::make_unique<GameInstance>(resources.window, resources.gameSave);
        palette = std::make_unique<es::World>();
        editor = std::make_unique<LevelEditor>(*gameInstance, stateEvent, *palette);
        selection = std::make_unique<SelectionGUI>(*gameInstance, resources.window, *palette);
        std::cout << "\nLevel editor is loaded.\n\n";
    }
}

void LevelEditorState::handleEvents()
{
    gameInstance->systems.update<InputSystem>(dt);
    for (auto& event: es::Events::get<sf::Event>())
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = ng::StateEvent::Exit;
        else
        {
            if (!selection->handleEvent(event))
                editor->handleEvent(event);
        }
    }
}

void LevelEditorState::update()
{
    bool withinBorder = selection->update(dt);
    editor->update(dt, withinBorder);
}

void LevelEditorState::draw()
{
    resources.window.clear(sf::Color(128, 128, 128));
    resources.window.draw(*editor);
    resources.window.draw(*selection);
    resources.window.display();
}
