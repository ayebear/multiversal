// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include <SFML/Graphics.hpp>
#include "nage/actions/actionhandler.h"
#include "nage/graphics/tilemap.h"
#include "level.h"

class GameInstance;
class Tile;
namespace ng { class StateEvent; }

/*
Handles the placing/editing/creating of tiles and world in a level.
Receives events from the TileSelection class to get what is currently selected.
Also receives events for handling focus.

Controls:
Left click: Place current tile or object
Right click: Remove tile or object
Shift+Left click: Enable state of object
Shift+Right click: Disable state of object
Ctrl+Left click: Edit tile
    Changes what the switches are connected to
    Left click: Enable tile/object connection (adds it to TileChanger component of switch object)
    Right click: Disable tile/object connection
    //Shift+left click: Enable object connection
    //Shift+right click: Disable object connection
*/
class LevelEditor: public sf::Drawable
{
    public:

        LevelEditor(GameInstance& gameInstance, ng::StateEvent& stateEvent, es::World& palette);
        void handleEvent(const sf::Event& event);
        void update(float dt, bool withinBorder);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:

        // Action callback methods
        void save();
        void load();
        void test();
        void undo();
        void redo();
        void clear();
        void escape();
        void toggleLayer();
        void nextLevel();
        void prevLevel();

        // Loads configuration file
        void loadConfig(const std::string& filename);

        // Mouse input handling
        void handleMouse(int tileId);
        void updateMousePos();
        void paintTile(int tileId, int visualId);
        bool getLocation();

        // Switch handling
        static const sf::Color switchColor;
        static const sf::Color tileConnectionColor;
        static const sf::Color objectConnectionColor;
        void connectSwitchToObject(int switchId, int tileId, bool connect, const sf::Color& color);
        void connectSwitchToTile(int switchId, int tileId, bool connect);
        void setBox(int tileId, bool show, const sf::Color& color);
        void updateBoxes();
        bool isObject(int tileId) const;
        bool isSwitch(int tileId) const;
        bool isControllable(int tileId) const;
        void changeSwitchMode(int tileId = -1);

        // Object handling
        void placeObject(int tileId);
        void removeObject(int tileId);
        void changeObjectState(int tileId, bool state);

        // Miscellaneous methods
        void updateBorder();
        void updateCurrentTile();
        void updateCurrentObject();
        void updateCurrentLayer();
        void resize(int deltaX, int deltaY);
        void initialize();

        GameInstance& gameInstance;
        ng::StateEvent& stateEvent;
        es::World& world;
        es::World& palette;

        // Settings
        static constexpr float panSpeed{2400.0f};
        static constexpr float defaultZoom{4.0f};

        // Other variables
        ng::ActionHandler actions;
        int currentLevel{1};
        int currentVisualId{0};
        int currentLayer{0};
        int currentTileId{0};
        sf::View view;
        sf::Vector2f mousePos;
        sf::Vector2u tileSize;
        sf::Vector2i location;
        bool shiftPressed{false};
        bool ctrlPressed{false};
        bool handledEvent{false};

        // The pallette of tiles that can be placed
        std::map<int, Tile> visualTiles;

        // Used for drawing sprites
        std::vector<sf::Sprite*> sprites;

        // Border to show boundaries
        sf::RectangleShape border;
        static const sf::Color borderColors[];

        // Current tile/object (hovers under mouse)
        enum class PlaceMode {Tile, Object} placeMode;
        bool showCurrent{false};
        ng::TileMap currentTile;
        es::Entity currentEntity{world};
        sf::Sprite currentEntitySprite;

        // Tile ID of current switch
        int selectedSwitch{-1};

        // Highlights what the switches are connected to
        std::map<int, sf::RectangleShape> boxes;

        // Tiles with inital on states
        es::Entity stateOnEnt{world};
};

#endif
