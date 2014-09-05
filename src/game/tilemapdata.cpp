// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "tilemapdata.h"

TileMapData::TileMapData()
{
    currentLayer = 0;
}

Tile& TileMapData::operator()(int layer, unsigned x, unsigned y)
{
    return tiles[layer](x, y);
}

Tile& TileMapData::operator()(unsigned x, unsigned y)
{
    return tiles[currentLayer](x, y);
}

void TileMapData::resize(unsigned width, unsigned height, bool preserve)
{
    for (auto& layer: tiles)
        layer.resize(width, height, preserve);
}

void TileMapData::useLayer(int layer)
{
    currentLayer = layer;
}
