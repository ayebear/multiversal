// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include "OCS/Objects.hpp"
#include <SFML/Graphics.hpp>

class TileMapData;
class TileMap;
class TileMapChanger;

/*
Handles the placing/editing/creating of tiles and objects in a level.
Receives events from the TileSelection class to get what is currently selected.
Also receives events for handling focus.
*/
class LevelEditor: public sf::Drawable
{
    public:
        LevelEditor(TileMapData& tileMapData, TileMap& tileMap, TileMapChanger& tileMapChanger, ocs::ObjectManager& entities);
        void handleEvent(const sf::Event& event);
        void update(float dt);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        TileMapData& tileMapData;
        TileMap& tileMap;
        TileMapChanger& tileMapChanger;
        ocs::ObjectManager& entities;

        int currentTileType;
        int currentLayer;
};

#endif
