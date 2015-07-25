// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "leveleditor.h"
#include "es/events.h"
#include "gameevents.h"
#include "gameinstance.h"
#include "configfile.h"
#include "nage/graphics/vectors.h"
#include "nage/states/stateevent.h"
#include "components.h"
#include "physicssystem.h"
#include "inaltworld.h"
#include "logicaltiles.h"
#include "gamesavehandler.h"
#include "nage/misc/utils.h"
#include "lasersystem.h"
#include "spritesystem.h"
#include "tilesmoothingsystem.h"
#include <iostream>

const sf::Color LevelEditor::borderColors[] = {sf::Color::Blue, sf::Color::Green};
const sf::Color LevelEditor::switchColor(255, 0, 0, 128);
const sf::Color LevelEditor::tileConnectionColor(0, 255, 0, 128);
const sf::Color LevelEditor::objectConnectionColor(0, 0, 255, 128);

LevelEditor::LevelEditor(GameInstance& gameInstance, ng::StateEvent& stateEvent, es::World& palette):
    gameInstance(gameInstance),
    stateEvent(stateEvent),
    world(gameInstance.world),
    palette(palette),
    placeMode(PlaceMode::Tile)
{
    view = gameInstance.camera.getView("window");
    view.zoom(defaultZoom);
    sf::FloatRect viewRect(-1116, -32, view.getSize().x, view.getSize().y);
    view.reset(viewRect);
    loadConfig("data/config/level_editor.cfg");
    tileSize = gameInstance.tileMap.getTileSize();
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

    load();
}

void LevelEditor::handleEvent(const sf::Event& event)
{
    actions.handleEvent(event);

    if (ctrlPressed && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        if (isSwitch(currentTileId))
            changeSwitchMode(currentTileId);
        else if (selectedSwitch == currentTileId)
            changeSwitchMode();
        handledEvent = true;
    }
}

void LevelEditor::update(float dt, bool withinBorder)
{
    updateMousePos();

    shiftPressed = actions["shift"].isActive();
    ctrlPressed = actions["control"].isActive();

    // Handle switching current tile (from selection GUI)
    for (auto& event: es::Events::get<TileSelectionEvent>())
    {
        placeMode = PlaceMode::Tile;
        currentVisualId = event.tileId;
        updateCurrentTile();
    }
    es::Events::clear<TileSelectionEvent>();

    // Handle switching current object (from Selection GUI)
    for (auto& event: es::Events::get<ObjectSelectionEvent>())
    {
        placeMode = PlaceMode::Object;
        currentEntity = palette[event.objectId];
        updateCurrentObject();
    }
    es::Events::clear<ObjectSelectionEvent>();

    // Handle mouse clicks with/without ctrl/shift
    if (currentTileId != -1 && !withinBorder && ng::Action::windowHasFocus && !handledEvent)
        handleMouse(currentTileId);
    handledEvent = false;

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
}

void LevelEditor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.setView(view);
    gameInstance.tileMap.drawLayer(target, currentLayer);
    gameInstance.smoothTileMap.drawLayer(target, currentLayer);

    for (auto ent: world.query<Sprite>())
    {
        if (inAltWorld(ent) == currentLayer)
            target.draw(ent.get<Sprite>()->sprite);
    }

    target.draw(border);

    // Draw selected tile/object under mouse
    if (showCurrent && selectedSwitch == -1)
    {
        if (placeMode == PlaceMode::Tile)
            target.draw(currentTile);
        else if (placeMode == PlaceMode::Object)
            target.draw(currentEntitySprite);
    }

    // Draw switch connections
    if (selectedSwitch != -1)
    {
        for (auto& box: boxes)
        {
            int layer = gameInstance.tileMapData.getLayer(box.first);
            if (layer == currentLayer)
                target.draw(box.second);
        }
    }
}

void LevelEditor::save()
{
    gameInstance.level.saveToFile(gameInstance.levelLoader.getLevelFilename(currentLevel));
}

void LevelEditor::load()
{
    gameInstance.level.loadFromFile(gameInstance.levelLoader.getLevelFilename(currentLevel));
    updateBorder();
    gameInstance.systems.initializeAll();
    gameInstance.systems.update<SpriteSystem>(1.0f / 60.0f);
    initialize();
}

void LevelEditor::test()
{
    // Push GameState and send a test event
    stateEvent.command = ng::StateEvent::Push;
    stateEvent.name = "Game";
    TestModeEvent event;
    gameInstance.level.saveToString(event.level);
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
    changeSwitchMode();
    gameInstance.level.clear();
    initialize();
}

void LevelEditor::escape()
{
    if (selectedSwitch == -1)
        stateEvent.command = ng::StateEvent::Pop;
    else
        changeSwitchMode();
}

void LevelEditor::toggleLayer()
{
    currentLayer = !currentLayer;
    updateBorder();
    updateCurrentLayer();
    es::Events::send(LayerChangedEvent{currentLayer});
}

void LevelEditor::nextLevel()
{
    ++currentLevel;
    ng::clamp(currentLevel, 1, GameSaveHandler::TOTAL_LEVELS);
    load();
}

void LevelEditor::prevLevel()
{
    --currentLevel;
    ng::clamp(currentLevel, 1, GameSaveHandler::TOTAL_LEVELS);
    load();
}

void LevelEditor::loadConfig(const std::string& filename)
{
    cfg::File config(filename);
    if (!config)
        return;

    // Load controls and setup actions
    actions.loadSection(config.getSection("Controls"), "");
    actions["growX"].setCallback([&]{ resize(1, 0); });
    actions["growY"].setCallback([&]{ resize(0, 1); });
    actions["shrinkX"].setCallback([&]{ resize(-1, 0); });
    actions["shrinkY"].setCallback([&]{ resize(0, -1); });
    ngBindAction(actions, save);
    ngBindAction(actions, load);
    ngBindAction(actions, test);
    ngBindAction(actions, undo);
    ngBindAction(actions, redo);
    ngBindAction(actions, clear);
    ngBindAction(actions, escape);
    ngBindAction(actions, toggleLayer);
    ngBindAction(actions, nextLevel);
    ngBindAction(actions, prevLevel);

    // Derive placeable tiles from the reverse lookup table
    for (const auto& data: gameInstance.tileMapData.getVisualToInfo())
    {
        int visualId = data.first;
        auto& info = data.second;

        // Setup tile
        auto& tile = visualTiles[visualId];
        tile.reset();
        tile.visualId = visualId;
        tile.logicalId = info.logicalId;
        tile.state = info.state;
        gameInstance.tileMapData.updateCollision(tile);
    }

    // Load the palette of entities
    Level::loadEntities(config.getSection("Entities"), palette);
    LaserSystem::updateRotations(palette);
    SpriteSystem::updateRotations(palette);
}

void LevelEditor::handleMouse(int tileId)
{
    if (ctrlPressed)
        return;

    bool leftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    bool rightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Right);

    if (selectedSwitch == -1)
    {
        // Normal state, not currently editing a switch
        if (shiftPressed && (leftPressed || rightPressed))
            changeObjectState(tileId, leftPressed);
        else if (placeMode == PlaceMode::Tile)
        {
            if (leftPressed)
                paintTile(tileId, currentVisualId);
            else if (rightPressed)
                paintTile(tileId, 0);
        }
        else if (placeMode == PlaceMode::Object)
        {
            if (leftPressed)
                placeObject(tileId);
            else if (rightPressed)
                removeObject(tileId);
        }
    }
    else if (leftPressed || rightPressed)
    {
        // Currently editing a switch tile
        if (shiftPressed)
            connectSwitchToObject(selectedSwitch, tileId, leftPressed, objectConnectionColor);
        else
            connectSwitchToTile(selectedSwitch, tileId, leftPressed);
    }
}

void LevelEditor::updateMousePos()
{
    for (auto& event: es::Events::get<MousePosEvent>())
    {
        mousePos = event.mousePos;
        showCurrent = getLocation();
        if (showCurrent)
        {
            // Update the positions of the current tile/object
            sf::Vector2f pos(location.x * tileSize.x, location.y * tileSize.y);
            currentTile.setPosition(pos);
            auto origin = currentEntitySprite.getOrigin();
            currentEntitySprite.setPosition(pos.x + origin.x, pos.y + origin.y);

            updateCurrentLayer();
        }
    }
}

void LevelEditor::paintTile(int tileId, int visualId)
{
    // Don't do anything if the current mouse position is out of bounds
    if (!showCurrent)
        return;

    // Set the tile and update the graphical tile map
    auto& tile = gameInstance.tileMapData(tileId);
    tile = visualTiles[visualId];

    // If it's a platform tile, erase it
    if (tile.visualId >= 0 && tile.visualId <= 2)
        tile.visualId = 0;

    // Update the tile smoothing system
    es::Events::send(PlatformTileUpdatedEvent{tileId});
    gameInstance.systems.update<TileSmoothingSystem>(0.01f);

    // Update graphical tile map
    gameInstance.tileMapChanger.updateVisualTile(tileId);

    // Add/remove tile ID to tile group component
    auto& tileIds = stateOnEnt.at<TileGroup>()->tileIds;
    if (tile.state)
        tileIds.insert(tileId);
    else
        tileIds.erase(tileId);

    // Remove the object at this tile ID
    world.destroy(std::to_string(tileId));
}

bool LevelEditor::getLocation()
{
    // Get location of tile to place, and make sure it is in bounds
    location.x = round((mousePos.x / static_cast<float>(tileSize.x)) - 0.5f);
    location.y = round((mousePos.y / static_cast<float>(tileSize.y)) - 0.5f);
    return gameInstance.tileMap.inBounds(location.x, location.y);
}

void LevelEditor::connectSwitchToObject(int switchId, int tileId, bool connect, const sf::Color& color)
{
    // ID of tile being connected to switch
    auto tileIdName = std::to_string(tileId);
    auto switchEnt = world("Switch", std::to_string(switchId));

    // Get switch component
    auto switchComp = switchEnt.get<Switch>();
    if (switchComp)
    {
        switchComp->tileId = switchId;

        // Look for the tile ID in the switch list
        auto& names = switchComp->objectNames;
        auto found = std::find(names.begin(), names.end(), tileIdName);

        if (connect && found == names.end())
        {
            // Add tile ID to switch list if it doesn't already exist
            names.push_back(tileIdName);
        }
        else if (!connect && found != names.end())
        {
            // Remove the tile ID from the switch list
            names.erase(found);
        }
    }

    setBox(tileId, connect, color);
}

void LevelEditor::connectSwitchToTile(int switchId, int tileId, bool connect)
{
    // Only connect to tiles that can be controlled
    if (connect && !isControllable(tileId))
        return;

    connectSwitchToObject(switchId, tileId, connect, tileConnectionColor);

    auto tileIdName = std::to_string(tileId);

    if (connect)
    {
        // Create TileController object at tile ID if it doesn't already exist
        auto tileEnt = world("TileController", tileIdName);
        tileEnt.at<TileGroup>()->tileIds.insert(tileId);
    }
    else
        world.destroy(tileIdName);
}

void LevelEditor::setBox(int tileId, bool show, const sf::Color& color)
{
    if (show)
    {
        // Set colors and size
        auto& box = boxes[tileId];
        box.setSize(ng::vec::cast<float>(tileSize));
        box.setFillColor(sf::Color::Transparent);
        box.setOutlineColor(color);
        box.setOutlineThickness(-16);

        // Set position
        unsigned x = gameInstance.tileMapData.getX(tileId);
        unsigned y = gameInstance.tileMapData.getY(tileId);
        box.setPosition(x * tileSize.x, y * tileSize.y);
    }
    else
        boxes.erase(tileId);
}

void LevelEditor::updateBoxes()
{
    if (selectedSwitch == -1)
        boxes.clear();
    else
    {
        // Setup box for switch itself
        setBox(selectedSwitch, true, switchColor);

        // Setup boxes for connections
        auto switchComp = world[std::to_string(selectedSwitch)].at<Switch>();
        for (auto& name: switchComp->objectNames)
        {
            // Get a TileGroup component (not an object if it has one)
            auto tileGroup = world.get(name).get<TileGroup>();
            if (tileGroup)
            {
                // Show tile connection(s)
                for (int tileId: tileGroup->tileIds)
                    setBox(tileId, true, tileConnectionColor);
            }
            else
            {
                // Show object connection
                int tileId = strlib::fromString<int>(name);
                setBox(tileId, true, objectConnectionColor);
            }
        }
    }
}

bool LevelEditor::isObject(int tileId) const
{
    auto ent = world.get(std::to_string(tileId));
    return (ent && !ent.has<TileGroup>());
}

bool LevelEditor::isSwitch(int tileId) const
{
    int logicalId = gameInstance.tileMapData(tileId).logicalId;
    return (logicalId == Tiles::PushButton ||
            logicalId == Tiles::ToggleSwitch ||
            logicalId == Tiles::LaserSensor);
}

bool LevelEditor::isControllable(int tileId) const
{
    int logicalId = gameInstance.tileMapData(tileId).logicalId;
    return (logicalId == Tiles::ConveyorBelt ||
            logicalId == Tiles::ForceField ||
            logicalId == Tiles::Mirror);
}

void LevelEditor::changeSwitchMode(int tileId)
{
    boxes.clear();
    selectedSwitch = tileId;
    updateBoxes();
    if (selectedSwitch == -1)
        std::cout << "Exiting switch mode\n";
    else
        std::cout << "Editing switch " << selectedSwitch << "\n";
}

void LevelEditor::placeObject(int tileId)
{
    auto name = std::to_string(tileId);
    if (!world.valid(name))
    {
        // Clone the entity from the palette
        auto ent = currentEntity.clone(world, name);

        // Setup tile position and other positions
        ent.at<TilePosition>()->id = tileId;
        gameInstance.systems.initialize<PhysicsSystem>();
        gameInstance.systems.update<SpriteSystem>(1.0f / 60.0f);

        std::cout << "Placed object '" << name << "'.\n";
    }
}

void LevelEditor::removeObject(int tileId)
{
    world.destroy(std::to_string(tileId));
}

void LevelEditor::changeObjectState(int tileId, bool state)
{
    auto stateComp = world.get(std::to_string(tileId)).get<State>();
    if (stateComp)
        stateComp->value = state;
}

void LevelEditor::updateBorder()
{
    border.setSize(ng::vec::cast<float>(gameInstance.tileMap.getPixelSize()));
    border.setOutlineColor(borderColors[currentLayer]);
}

void LevelEditor::updateCurrentTile()
{
    currentTile.set(0, 0, currentVisualId);
}

void LevelEditor::updateCurrentObject()
{
    // Use the current object ID to get the sprite component
    // Copy the sprite to avoid memory issues
    auto spriteComp = currentEntity.get<Sprite>();
    if (spriteComp)
    {
        currentEntitySprite = spriteComp->sprite;
        //currentEntitySprite.setTexture(spriteComp->sprite.getTexture());
    }
}

void LevelEditor::updateCurrentLayer()
{
    // Update the current tile ID being hovered over
    currentTileId = gameInstance.tileMapData.getId(currentLayer, location.x, location.y);
}

void LevelEditor::resize(int deltaX, int deltaY)
{
    auto mapSize = ng::vec::cast<int>(gameInstance.tileMap.getMapSize());
    gameInstance.tileMapChanger.resize(mapSize.x + deltaX, mapSize.y + deltaY);
    updateBorder();
}

void LevelEditor::initialize()
{
    // Setup "onStates" entity
    stateOnEnt = world("TileController", "onStates");
    stateOnEnt.at<TileGroup>()->initialState = true;
}
