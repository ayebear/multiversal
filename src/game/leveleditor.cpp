// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "leveleditor.h"
#include "events.h"
#include "gameevents.h"
#include "tilemap.h"
#include "tilemapdata.h"
#include "tilemapchanger.h"

LevelEditor::LevelEditor(TileMapData& tileMapData, TileMap& tileMap, TileMapChanger& tileMapChanger, ocs::ObjectManager& entities):
    tileMapData(tileMapData),
    tileMap(tileMap),
    tileMapChanger(tileMapChanger),
    entities(entities)
{
}

void LevelEditor::handleEvent(const sf::Event& event)
{
    // Handle switching between layers (spacebar?)
}

void LevelEditor::update(float dt)
{
    // Handle clicking (paint tiles)
    for (auto& event: es::Events::get<MouseClickedEvent>())
    {
        // Get location of tile to place
        auto tileSize = tileMap.getTileSize();
        sf::Vector2i location(event.mousePos.x / tileSize.x, event.mousePos.y / tileSize.y);
        int tileId = tileMapData.getId(currentLayer, location.x, location.y);

        // Update the logical and visual information
        tileMapData(tileId).logicalId = currentTileType;
        tileMapChanger.updateTile(tileId);
    }

    // Handle panning with arrow keys
}

void LevelEditor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

}
