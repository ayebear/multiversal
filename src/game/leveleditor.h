// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include "OCS/Objects.hpp"
#include <SFML/Graphics.hpp>
#include "nage/actions/actionhandler.h"
#include "nage/graphics/tilemap.h"
#include "level.h"

class GameWorld;
class Tile;
namespace ng { class StateEvent; }

/*
Handles the placing/editing/creating of tiles and objects in a level.
Receives events from the TileSelection class to get what is currently selected.
Also receives events for handling focus.
*/
class LevelEditor: public sf::Drawable
{
    public:

        LevelEditor(GameWorld& world, ng::StateEvent& stateEvent);
        void handleEvent(const sf::Event& event);
        void update(float dt);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:

        // Action callback methods
        void save();
        void load();
        void test();
        void undo();
        void redo();
        void clear();
        void nextLevel();
        void prevLevel();

        void loadConfig(const std::string& filename);
        void updateMousePos();
        void paintTile(int visualId);
        void updateBorder();
        void updateCurrentTile();
        void resize(int deltaX, int deltaY);

        GameWorld& world;
        ng::StateEvent& stateEvent;

        // Settings
        float panSpeed = 100;
        float defaultZoom = 1;

        // Other variables
        ng::ActionHandler actions;
        int currentLevel = 1;
        int currentVisualId = 0;
        int currentLayer = 0;
        std::map<int, Tile> visualTiles; // Used to place tiles
        sf::View view;
        sf::Vector2f mousePos;
        sf::RectangleShape border; // Border to show boundaries
        sf::Vector2u tileSize;

        // Current tile (hovers under mouse)
        ng::TileMap currentTile;

        // Object palette and placing
        ocs::ObjectManager entities;
        Level::ObjectNameMap objectNames;
};

#endif
