// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef TILEMAPDATA_H
#define TILEMAPDATA_H

#include "matrix.h"

struct Tile
{
    int logicalId;
    int visualId;
    bool collidable;
    bool state;
};

class TileMapData
{
    public:
        TileMapData();
        Tile& operator()(int layer, unsigned x, unsigned y);
        Tile& operator()(unsigned x, unsigned y);
        void resize(unsigned width, unsigned height, bool preserve = true);
        void useLayer(int layer);

    private:
        Matrix<Tile> tiles[2];
        int currentLayer;
};

#endif
