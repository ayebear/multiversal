// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELEDITORSTATE_H
#define LEVELEDITORSTATE_H

#include "nage/states/basestate.h"
#include "leveleditor.h"
#include "selectiongui.h"
#include "gameworld.h"
#include "objectpalette.h"
#include <memory>

class GameResources;

/*
ObjectPalette
    For storing the objects that can be placed
    This is used by both the LevelEditor and SelectionGUI
LevelEditor
    For placing tiles/objects in the level
SelectionGUI
    For choosing tiles/objects
    Sends events to the editor for the currently selected tile/object
*/
class LevelEditorState: public ng::BaseState
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
        std::unique_ptr<ObjectPalette> objectPalette;
        std::unique_ptr<LevelEditor> editor;
        std::unique_ptr<SelectionGUI> selection;
};

#endif
