// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef TILEMAPCHANGER_H
#define TILEMAPCHANGER_H

#include <functional>

class TileMapData;
namespace ng { class TileMap; }

/*
A simplified way to change the logical and visual tilemap simultaneously.
*/
class TileMapChanger
{
    public:

        TileMapChanger(TileMapData& tileMapData, ng::TileMap& tileMap);

        // Returns true if the state of the tile changed from this call
        bool changeState(int tileId, bool state);

        // Toggles the state of a tile
        void toggleState(int tileId);

        // Derives other tile layer information
        void updateVisualTile(int tileId);

        // Resizes both tile maps, and re-populates the visual tiles
        void resize(int width, int height);

        // Resets all of the tiles, but does not resize anything
        void clear();

    private:

        // Applies a function to every tile
        using FuncType = std::function<void(unsigned, unsigned)>;
        void apply(FuncType callback);

        TileMapData& tileMapData;
        ng::TileMap& tileMap;
};

#endif
