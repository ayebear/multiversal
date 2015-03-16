// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELEDITORSTATE_H
#define LEVELEDITORSTATE_H

#include "basestate.h"
#include "leveleditor.h"
#include "tileselection.h"
#include <SFML/Graphics.hpp>

class GameObjects;
class Game;
class GameWorld;

/*
LevelEditor -> For placing tiles/objects in the level
    TileMap
    View
TileSelection -> For choosing tiles/objects
    TileMap
    View
    RenderTexture
The selector sends events to the editor for the currently selected tile/object.
SystemContainer& -> For the systems for the objects
*/
class LevelEditorState: public BaseState
{
    public:
        LevelEditorState(GameObjects& objects, Game& game);

        void handleEvents();
        void update();
        void draw();

    private:
        GameObjects& objects;
        Game& game;
        GameWorld& world;
        LevelEditor editor;
        TileSelection selection;
};

#endif
