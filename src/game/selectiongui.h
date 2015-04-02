// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef SELECTIONGUI_H
#define SELECTIONGUI_H

#include <SFML/Graphics.hpp>
#include "nage/gui/buttonmap.h"
#include "nage/graphics/tilemap.h"

class GameWorld;

/*
A scrollable widget of tiles and objects that can be chosen from.
The chosen tile/object is sent to the level editor as an event.
*/
class SelectionGUI: public sf::Drawable
{
    public:
        SelectionGUI(GameWorld& world, sf::RenderWindow& window);
        bool handleEvent(const sf::Event& event);
        void update(float dt);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        void setupTiles();
        void setupObjects();
        bool withinBorder(const sf::Vector2f& pos) const;
        bool withinTexture(const sf::Vector2f& pos) const;
        void handleTilesTab();
        void handleObjectsTab();
        void handleMouseEvent(const sf::Vector2i& pos, bool clicked);

        // Takes a mouse position, and selects a tile/object
        void select(const sf::Vector2f& pos, bool clicked);

        enum class TabState
        {
            Tiles,
            Objects
        };

        static const unsigned TILE_COLUMNS = 4;
        static const unsigned WIDTH_PIXELS = 320;
        static const unsigned PADDING = 4;
        static const unsigned BUTTON_WIDTH = 156;
        static const unsigned BUTTON_HEIGHT = 40;

        GameWorld& world;
        sf::RenderWindow& window;

        sf::Vector2i rawMousePos;
        sf::Vector2f guiMousePos;
        sf::Vector2f textureMousePos;
        sf::View view;
        sf::View textureView;
        sf::Font font;
        sf::Vector2f borderSize;
        sf::RectangleShape border;
        sf::RectangleShape currentSelection;
        sf::RectangleShape hoverSelection;
        ng::ButtonMap tabs;
        TabState state{TabState::Tiles};

        // Render texture for tiles/objects
        sf::RenderTexture texture;
        sf::Vector2f texturePos;
        sf::Sprite sprite;

        // Selectable tiles
        ng::TileMap tiles;
        sf::Vector2u tileSize;
        sf::Vector2u tileCount; // Size in # of tiles
};

#endif
