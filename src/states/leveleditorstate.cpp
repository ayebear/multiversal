// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "leveleditorstate.h"
#include "gameobjects.h"
#include "game.h"

LevelEditorState::LevelEditorState(GameObjects& objects, Game& game):
    objects(objects),
    game(game),
    world(game.getWorld()),
    editor(world.tileMapData, world.tileMap, world.tileMapChanger, world.entities)
{
}

void LevelEditorState::handleEvents()
{
    sf::Event event;
    while (objects.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = StateEvent::Exit;
        else
        {
            editor.handleEvent(event);
            selection.handleEvent(event);
        }
    }
}

void LevelEditorState::update()
{
    editor.update(dt);
    selection.update(dt);
}

void LevelEditorState::draw()
{
    objects.window.draw(editor);
    objects.window.draw(selection);
}
