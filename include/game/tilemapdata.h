// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef TILEMAPDATA_H
#define TILEMAPDATA_H

#include <map>
#include <set>
#include <unordered_map>
#include <SFML/System/Vector2.hpp>
#include "nage/misc/matrix.h"

// Holds all of the information for a tile
struct Tile
{
    int logicalId{};
    int visualId{};
    bool collidable{};
    bool blocksLaser{};
    bool state{};

    void reset();
};

/*
A logical tile map in memory (not graphical)
Note that this is specific to the game.
*/
class TileMapData
{
    public:

        TileMapData();

        // Access tiles
        Tile& operator()(int layer, unsigned x, unsigned y);
        Tile& operator()(unsigned x, unsigned y);
        Tile& operator()(int id);

        // Access tiles (const)
        const Tile& operator()(int layer, unsigned x, unsigned y) const;
        const Tile& operator()(unsigned x, unsigned y) const;
        const Tile& operator()(int id) const;

        // Size and layers
        void resize(unsigned width, unsigned height, bool preserve = true);
        unsigned width() const;
        unsigned height() const;
        sf::Vector2u size() const;
        bool inBounds(int x, int y) const;
        bool inBounds(int tileId) const;
        void useLayer(int layer);

        // Compute tile IDs
        int getId(unsigned x, unsigned y) const;
        int getId(int layer, unsigned x, unsigned y) const;

        // Lookup/derive tile information
        void deriveTiles();
        void updateVisualId(int id);
        void updateVisualId(Tile& tile);
        void updateCollision(int id);
        void updateCollision(Tile& tile);
        void updateState(int id);
        void updateState(Tile& tile);

        // Tiles with world on top
        void addTile(int id);
        bool findTile(int id) const;
        void clearTiles();

        // Tile IDs
        using TileList = std::vector<int>;
        TileList& operator[](int logicalId);
        void clearTileIds();

        // These are needed to extract information from the ID
        int getLayer(int id) const;
        unsigned getX(int id) const;
        unsigned getY(int id) const;

        // Returns a reference to the visual ID reverse lookup table
        inline const auto& getVisualToInfo() const
        {
            return visualToInfo;
        }

    private:

        // Populates the maps from a config file
        void loadTileInfo();


        // Level specific ----------------------------------------------------

        // Tile map
        ng::Matrix<Tile> tiles[2];
        int currentLayer;

        // Map of logical IDs to lists of tile IDs
        std::map<int, TileList> tileIds;

        // All of the tile IDs with world currently on top of them
        std::set<int> tilesWithObjectsOnTop;


        // Game specific -----------------------------------------------------

        struct TileInfo
        {
            enum
            {
                Collision = 0,
                CollisionTrue,
                LaserCollision,
                LaserCollisionTrue,
                StateToVisual,
                StateToVisualTrue
            };

            bool collision[4]{};
            int stateToVisual[2]{};
            bool stateToVisualUsed{false};
        };

        // Used to lookup information about a particular logical ID (tile type)
        std::unordered_map<int, TileInfo> logicalToInfo;

        // Visual ID -> Logical ID lookup
        struct VisualInfo
        {
            int logicalId{};
            bool state{false};
        };
        std::unordered_map<int, VisualInfo> visualToInfo;
};

#endif
