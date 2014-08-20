// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <map>
#include <SFML/Graphics.hpp>

/*
TODO:
    Change the order of stored vertices so they are contiguous (left to right)
*/

/*
This class handles storing and drawing a visual map of tiles.
Tiles are represented with an ID
The tile size and map size can be set.
    Note: Changing this after setting the tiles will reset everything.
A tileset (in a single texture) must be specified with the tile size, as well as the
    total number of tiles. The rest is automatically calculated (tiles per row, etc.)
It uses vertex arrays for performance.
    Note: This means that only one texture can be used.
Multiple layers are supported, and you can use your own integer IDs for each layer.
Animation support could be added in the future if needed.
*/
class TileMap: public sf::Drawable, public sf::Transformable
{
    public:
        TileMap();
        bool loadTileset(const std::string& filename, unsigned tileWidth, unsigned tileHeight);
        void resize(unsigned width, unsigned height);
        void useLayer(int layer); // Sets the "current" layer
        void set(int layer, unsigned x, unsigned y, unsigned value);
        void set(unsigned x, unsigned y, unsigned value);
        unsigned operator()(int layer, unsigned x, unsigned y) const;
        unsigned operator()(unsigned x, unsigned y) const;
        sf::Vector2u getPixelSize() const;
        const sf::Vector2u& getMapSize() const;
        const sf::Vector2u& getTileSize() const;
        sf::FloatRect getBoundingBox(unsigned x, unsigned y) const;
        sf::Vector2u getCenterPoint(unsigned x, unsigned y) const;
        void drawLayer(sf::RenderTarget& target, int layer);
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        struct TileLayer
        {
            sf::VertexArray vertices; // Graphical tiles
            std::vector<unsigned> tiles; // Tile IDs
        };

        void resize();
        void resize(TileLayer& layer);
        bool inBounds(unsigned x, unsigned y) const;

        unsigned totalTiles;
        sf::Vector2u mapSize; // In # of tiles
        sf::Vector2u tileSize; // In pixels
        sf::Texture texture; // The tile set
        std::map<int, TileLayer> tiles;
        TileLayer* currentLayer; // Points to the last layer used
};

#endif
