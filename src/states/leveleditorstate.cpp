// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "leveleditorstate.h"
#include "gameobjects.h"

LevelEditorState::LevelEditorState(GameObjects& objects):
    objects(objects)
{
}

void LevelEditorState::handleEvents()
{
    sf::Event event;
    while (objects.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = StateEvent::Exit;
    }
}

void LevelEditorState::update()
{
    // Temporarily disable this state until it is implemented
    stateEvent.command = StateEvent::Pop;
}

void LevelEditorState::draw()
{
}
