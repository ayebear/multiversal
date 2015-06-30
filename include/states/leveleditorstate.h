// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELEDITORSTATE_H
#define LEVELEDITORSTATE_H

#include "nage/states/basestate.h"
#include "leveleditor.h"
#include "selectiongui.h"
#include "gameinstance.h"
#include <memory>

class GameResources;

/*
Palette
    For storing the entities that can be placed
    This is used by both the LevelEditor and SelectionGUI
LevelEditor
    For placing tiles/entities in the level
SelectionGUI
    For choosing tiles/entities
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
        std::unique_ptr<GameInstance> gameInstance;
        std::unique_ptr<es::World> palette;
        std::unique_ptr<LevelEditor> editor;
        std::unique_ptr<SelectionGUI> selection;
};

#endif
