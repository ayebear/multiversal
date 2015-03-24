// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELEDITORSTATE_H
#define LEVELEDITORSTATE_H

#include "nage/states/basestate.h"
#include "leveleditor.h"
#include "tileselection.h"
#include "gameworld.h"
#include <memory>

class GameResources;

/*
LevelEditor -> For placing tiles/objects in the level
    TileMap
    View
TileSelection -> For choosing tiles/objects
    TileMap
    View (default)
    View (for render texture)
    RenderTexture
The selector sends events to the editor for the currently selected tile/object.
SystemContainer& -> For the systems for the objects
*/
class LevelEditorState: public BaseState
{
    public:
        LevelEditorState(GameResources& resources);

        void onStart();
        void handleEvents();
        void update();
        void draw();

    private:
        GameResources& resources;
        std::unique_ptr<GameWorld> world;
        std::unique_ptr<LevelEditor> editor;
        std::unique_ptr<TileSelection> selection;
};

#endif
