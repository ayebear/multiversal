// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "leveleditor.h"
#include "events.h"
#include "gameevents.h"
#include "gameworld.h"
#include "configfile.h"
#include "nage/graphics/vectors.h"
#include "nage/states/stateevent.h"
#include "componentstrings.h"
#include "entityprototypeloader.h"
#include "components.h"
#include "spritesystem.h"
#include "inaltworld.h"

const sf::Color LevelEditor::borderColors[] = {sf::Color::Green, sf::Color::Blue};

LevelEditor::LevelEditor(GameWorld& world, ng::StateEvent& stateEvent):
    world(world),
    stateEvent(stateEvent)
{
    view = world.camera.getView("window");
    view.zoom(defaultZoom);
    sf::FloatRect viewRect(-1116, -32, view.getSize().x, view.getSize().y);
    view.reset(viewRect);
    loadConfig("data/config/level_editor.cfg");
    tileSize = world.tileMap.getTileSize();
    resize(32, 12);

    // Setup border
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(24);
    updateBorder();

    // Using a TileMap to display the current tile
    // TODO: Make something simpler to use tiles from tile sets
    currentTile.loadFromConfig("data/config/tilemap.cfg");
    currentTile.resize(1, 1);
    currentTile.useLayer(0);
    currentTile.setColor(sf::Color(255, 255, 255, 128));
    currentVisualId = 1;
    updateCurrentTile();

    initialize();
}

void LevelEditor::handleEvent(const sf::Event& event)
{
    actions.handleEvent(event);
}

void LevelEditor::update(float dt, bool withinBorder)
{
    updateMousePos();

    // Handle switching current tile (from selection GUI)
    for (auto& event: es::Events::get<TileSelectionEvent>())
    {
        currentVisualId = event.tileId;
        updateCurrentTile();
    }
    es::Events::clear<TileSelectionEvent>();

    // Handle painting tiles
    if (!withinBorder && ng::Action::windowHasFocus)
    {
        bool leftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        bool rightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Right);
        if (leftPressed)
            paintTile(currentVisualId);
        else if (rightPressed)
            paintTile(0);
    }

    // Handle panning
    sf::Vector2f panDelta;
    if (actions["panLeft"].isActive())
        panDelta.x -= 1;
    if (actions["panRight"].isActive())
        panDelta.x += 1;
    if (actions["panUp"].isActive())
        panDelta.y -= 1;
    if (actions["panDown"].isActive())
        panDelta.y += 1;
    view.move(panDelta.x * panSpeed * dt, panDelta.y * panSpeed * dt);

    // Handle zooming
    if (actions["zoomIn"].isActive())
        view.zoom(1.0f - dt);
    if (actions["zoomOut"].isActive())
        view.zoom(1.0f + dt);

    // Update the view for the input system
    es::Events::send(ViewEvent{view});

    // Save sprites in the current layer to draw later
    sprites.clear();
    for (auto& sprite: world.objects.getComponentArray<Components::Sprite>())
    {
        if (inAltWorld(world.objects, sprite.getOwnerID()) == currentLayer)
            sprites.push_back(&sprite.sprite);
    }
}

void LevelEditor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.setView(view);
    world.tileMap.drawLayer(target, currentLayer);

    for (auto sprite: sprites)
        target.draw(*sprite);

    target.draw(border);
    target.draw(currentTile);
}

void LevelEditor::save()
{
    world.level.saveToFile("data/levels/test.cfg");
}

void LevelEditor::load()
{
    world.level.loadFromFile(world.levelLoader.getLevelFilename(currentLevel));
    updateBorder();
    world.systems.initializeAll();
    world.systems.update<SpriteSystem>(1.0f / 60.0f);
    initialize();
}

void LevelEditor::test()
{
    // Push GameState and send a test event
    stateEvent.command = ng::StateEvent::Push;
    stateEvent.name = "Game";
    TestModeEvent event;
    world.level.saveToString(event.level);
    es::Events::send(event);
}

void LevelEditor::undo()
{
    // Use an "UndoStack" class of some kind
    // Have "actions" that can be stored and applied/unapplied with before/after values
}

void LevelEditor::redo()
{

}

void LevelEditor::clear()
{
    world.level.clear();
}

void LevelEditor::toggleLayer()
{
    currentLayer = !currentLayer;
    updateBorder();
}

void LevelEditor::nextLevel()
{
    ++currentLevel;
    load();
}

void LevelEditor::prevLevel()
{
    --currentLevel;
    load();
}

void LevelEditor::loadConfig(const std::string& filename)
{
    cfg::File config(filename, cfg::File::Errors);
    if (!config)
        return;

    // Load controls and setup actions
    actions.loadSection(config.getSection("Controls"), "");
    actions["popState"].setCallback([&]{ stateEvent.command = ng::StateEvent::Pop; });
    actions["growX"].setCallback([&]{ resize(1, 0); });
    actions["growY"].setCallback([&]{ resize(0, 1); });
    actions["shrinkX"].setCallback([&]{ resize(-1, 0); });
    actions["shrinkY"].setCallback([&]{ resize(0, -1); });
    ng_bindAction(actions, save);
    ng_bindAction(actions, load);
    ng_bindAction(actions, test);
    ng_bindAction(actions, undo);
    ng_bindAction(actions, redo);
    ng_bindAction(actions, clear);
    ng_bindAction(actions, toggleLayer);
    ng_bindAction(actions, nextLevel);
    ng_bindAction(actions, prevLevel);

    // Load visual tile reverse lookup table
    for (auto& option: config.getSection("VisualTiles"))
    {
        int visualId = strlib::fromString<int>(option.first);
        auto values = strlib::split<int>(option.second, " ");

        // Setup tile
        auto& tile = visualTiles[visualId];
        tile.reset();
        tile.visualId = visualId;

        // Extract information
        if (!values.empty())
        {
            tile.logicalId = values.front();
            if (values.size() >= 2)
                tile.state = (values[1] != 0);
            world.tileMapData.updateCollision(tile);
        }
    }

    // Load the object palette
    bindComponentStrings(objectPalette);
    if (es::EntityPrototypeLoader::load(objectPalette, "data/config/objects.cfg"))
        world.level.loadObjects(config.getSection("Objects"), objectPalette, objectNamesPalette, false);
    else
        std::cerr << "ERROR: Could not load object prototypes.\n";
}

void LevelEditor::updateMousePos()
{
    for (auto& event: es::Events::get<MousePosEvent>())
    {
        mousePos = event.mousePos;
        currentTile.setPosition(sf::Vector2f(int(mousePos.x / int(tileSize.x)) * tileSize.x,
                                             int(mousePos.y / int(tileSize.y)) * tileSize.y));
    }
}

void LevelEditor::paintTile(int visualId)
{
    // Get location of tile to place, and make sure it is in bounds
    sf::Vector2i location(mousePos.x / int(tileSize.x), mousePos.y / int(tileSize.y));
    if (world.tileMap.inBounds(location.x, location.y))
    {
        int tileId = world.tileMapData.getId(currentLayer, location.x, location.y);
        //std::cout << "Location: " << tileId << ", Type: " << visualId << "\n";

        // Set the tile and update the graphical tile map
        auto& tile = world.tileMapData(tileId);
        tile = visualTiles[visualId];
        world.tileMapChanger.updateVisualTile(tileId);

        // Add tile ID to tile group component
        auto objectId = (tile.state ? stateOnId : stateOffId);
        auto comp = world.objects.getComponent<Components::TileGroup>(objectId);
        if (comp)
            comp->tileIds.insert(tileId);
    }
}

void LevelEditor::updateBorder()
{
    border.setSize(ng::vectors::cast<float>(world.tileMap.getPixelSize()));
    border.setOutlineColor(borderColors[currentLayer]);
}

void LevelEditor::updateCurrentTile()
{
    currentTile.set(0, 0, currentVisualId);
}

void LevelEditor::resize(int deltaX, int deltaY)
{
    auto mapSize = ng::vectors::cast<int>(world.tileMap.getMapSize());
    world.tileMapChanger.resize(mapSize.x + deltaX, mapSize.y + deltaY);
    updateBorder();
}

void LevelEditor::initialize()
{
    // Add TileChanger objects for initial on/off states of tiles
    stateOnId = world.objects.createObject("TileController");
    stateOffId = world.objects.createObject("TileController");

    // Set initial state
    auto comp = world.objects.getComponent<Components::TileGroup>(stateOnId);
    if (comp)
        comp->initialState = true;

    world.level.registerObjectName(stateOnId, "onStates");
    world.level.registerObjectName(stateOffId, "offStates");
}
