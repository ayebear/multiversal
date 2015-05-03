// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "leveleditor.h"
#include "es/events.h"
#include "gameevents.h"
#include "gameworld.h"
#include "configfile.h"
#include "nage/graphics/vectors.h"
#include "nage/states/stateevent.h"
#include "components.h"
#include "spritesystem.h"
#include "physicssystem.h"
#include "inaltworld.h"
#include "logicaltiles.h"
#include "gamesavehandler.h"
#include "nage/misc/utils.h"
#include "objectpalette.h"

const sf::Color LevelEditor::borderColors[] = {sf::Color::Green, sf::Color::Blue};
const sf::Color LevelEditor::switchColor(255, 0, 0, 128);
const sf::Color LevelEditor::connectionColor(0, 255, 0, 128);

LevelEditor::LevelEditor(GameWorld& world, ng::StateEvent& stateEvent, ObjectPalette& objectPalette):
    world(world),
    stateEvent(stateEvent),
    objectPalette(objectPalette),
    placeMode(PlaceMode::Tile),
    currentObject(-1)
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
        currentObject = event.objectId;
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

    // Draw selected tile/object under mouse
    if (showCurrent && selectedSwitch == -1)
    {
        if (placeMode == PlaceMode::Tile)
            target.draw(currentTile);
        else if (placeMode == PlaceMode::Object)
            target.draw(currentObjectSprite);
    }

    // Draw switch connections
    if (selectedSwitch != -1)
    {
        for (auto& box: boxes)
        {
            int layer = world.tileMapData.getLayer(box.first);
            if (layer == currentLayer)
                target.draw(box.second);
        }
    }
}

void LevelEditor::save()
{
    world.level.saveToFile(world.levelLoader.getLevelFilename(currentLevel));
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
    changeSwitchMode();
    world.level.clear();
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
    cfg::File config(filename, cfg::File::Errors);
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
    for (const auto& data: world.tileMapData.getVisualToInfo())
    {
        int visualId = data.first;
        auto& info = data.second;

        // Setup tile
        auto& tile = visualTiles[visualId];
        tile.reset();
        tile.visualId = visualId;
        tile.logicalId = info.logicalId;
        tile.state = info.state;
        world.tileMapData.updateCollision(tile);
    }

    // Load the object palette
    objectPalette.load(config.getSection("Objects"), world.level);
}

void LevelEditor::handleMouse(int tileId)
{
    bool leftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    bool rightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Right);

    if (selectedSwitch == -1)
    {
        // Normal state, not currently editing a switch
        if (!ctrlPressed)
        {
            if (placeMode == PlaceMode::Tile)
            {
                if (leftPressed)
                    paintTile(tileId, currentVisualId);
                else if (rightPressed)
                    paintTile(tileId, 0);
            }
            else
            {
                if (leftPressed)
                    placeObject(tileId);
                else if (rightPressed)
                    removeObject(tileId);
            }
        }
    }
    else
    {
        // Currently editing a switch tile
        if (shiftPressed)
        {
            if (leftPressed || rightPressed)
                changeObjectState(tileId, leftPressed);
        }
        else if (!ctrlPressed)
        {
            // Enable/disable tile
            if (leftPressed)
                setSwitchConnection(selectedSwitch, tileId, true);
            else if (rightPressed)
                setSwitchConnection(selectedSwitch, tileId, false);
        }
    }
}

void LevelEditor::updateMousePos()
{
    for (auto& event: es::Events::get<MousePosEvent>())
    {
        mousePos = event.mousePos;
        sf::Vector2i location;
        showCurrent = getLocation(location);
        if (showCurrent)
        {
            // Update the positions of the current tile/object
            sf::Vector2f pos(location.x * tileSize.x, location.y * tileSize.y);
            currentTile.setPosition(pos);
            auto origin = currentObjectSprite.getOrigin();
            currentObjectSprite.setPosition(pos.x + origin.x, pos.y + origin.y);

            // Update the current tile ID being hovered over
            currentTileId = world.tileMapData.getId(currentLayer, location.x, location.y);
        }
    }
}

void LevelEditor::paintTile(int tileId, int visualId)
{
    // Set the tile and update the graphical tile map
    auto& tile = world.tileMapData(tileId);
    tile = visualTiles[visualId];
    world.tileMapChanger.updateVisualTile(tileId);

    // Add/remove tile ID to tile group component
    auto comp = world.objects.getComponent<Components::TileGroup>(stateOnId);
    if (comp)
    {
        if (tile.state)
            comp->tileIds.insert(tileId);
        else
            comp->tileIds.erase(tileId);
    }
}

bool LevelEditor::getLocation(sf::Vector2i& location) const
{
    // Get location of tile to place, and make sure it is in bounds
    location.x = round((mousePos.x / static_cast<float>(tileSize.x)) - 0.5f);
    location.y = round((mousePos.y / static_cast<float>(tileSize.y)) - 0.5f);
    return world.tileMap.inBounds(location.x, location.y);
}

auto LevelEditor::setupSwitch(int switchId)
{
    auto switchIdName = std::to_string(switchId);
    auto switchObj = world.level.getObjectIdFromName(switchIdName);

    // Create new switch object if it doesn't already exist
    if (switchObj == ocs::invalidID)
    {
        switchObj = world.objects.createObject("Switch");
        world.level.registerObjectName(switchObj, switchIdName);
    }

    return switchObj;
}

void LevelEditor::setSwitchConnection(int switchId, int tileId, bool connect)
{
    // Only connect to tiles that can be controlled
    if (!isControllable(tileId))
        return;

    auto tileIdName = std::to_string(tileId); // ID of tile being connected to switch
    auto switchObj = setupSwitch(switchId);

    // Get switch component
    auto switchComp = world.objects.getComponent<Components::Switch>(switchObj);
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

    // Create TileController object at tile ID if it doesn't already exist
    auto tileObj = world.level.getObjectIdFromName(tileIdName);
    if (tileObj == ocs::invalidID)
    {
        tileObj = world.objects.createObject("TileController");
        world.level.registerObjectName(tileObj, tileIdName);

        // Add tile ID to TileGroup component
        auto tileGroup = world.objects.getComponent<Components::TileGroup>(tileObj);
        if (tileGroup)
            tileGroup->tileIds.insert(tileId);
    }

    setBox(tileId, connect, connectionColor);
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
        unsigned x = world.tileMapData.getX(tileId);
        unsigned y = world.tileMapData.getY(tileId);
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
        // Get switch component
        auto switchId = world.level.getObjectIdFromName(std::to_string(selectedSwitch));
        auto switchComp = world.objects.getComponent<Components::Switch>(switchId);

        // Setup box for switch itself
        setBox(selectedSwitch, true, switchColor);

        // Setup boxes for connections
        if (switchComp)
        {
            // Go through connected objects
            for (auto& name: switchComp->objectNames)
            {
                auto id = world.level.getObjectIdFromName(name);
                if (id != ocs::invalidID)
                {
                    // Get a TileGroup component
                    auto tileGroup = world.objects.getComponent<Components::TileGroup>(id);
                    if (tileGroup)
                    {
                        // Go through tile IDs of TileGroup component
                        for (int tileId: tileGroup->tileIds)
                            setBox(tileId, true, connectionColor);
                    }
                }
            }
        }
    }
}

bool LevelEditor::isSwitch(int tileId) const
{
    int logicalId = world.tileMapData(tileId).logicalId;
    return (logicalId == Tiles::PushButton ||
            logicalId == Tiles::ToggleSwitch ||
            logicalId == Tiles::LaserSensor);
}

bool LevelEditor::isControllable(int tileId) const
{
    int logicalId = world.tileMapData(tileId).logicalId;
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
    if (world.level.getObjectIdFromName(name) == ocs::invalidID)
    {
        auto id = objectPalette.copyObject(currentObject, world.objects);
        world.level.registerObjectName(id, name);

        // TODO: Use "assign" when it's available
        if (!world.objects.hasComponents<Components::TilePosition>(id))
            world.objects.addComponents<Components::TilePosition>(id, Components::TilePosition());

        auto tilePos = world.objects.getComponent<Components::TilePosition>(id);
        if (tilePos)
        {
            tilePos->id = tileId;

            // Update rest of tile position and other positions
            world.systems.initialize<PhysicsSystem>();
            world.systems.update<SpriteSystem>(1.0f / 60.0f);
        }
        std::cout << "Placed object " << name << "\n";
    }
}

void LevelEditor::removeObject(int tileId)
{
    auto name = std::to_string(tileId);
    auto id = world.level.getObjectIdFromName(name);
    if (id != ocs::invalidID)
    {
        world.objects.destroyObject(id);
        world.level.unregisterObjectName(name);
    }
}

void LevelEditor::changeObjectState(int tileId, bool state)
{

}

void LevelEditor::updateBorder()
{
    border.setSize(ng::vec::cast<float>(world.tileMap.getPixelSize()));
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
    auto spriteComp = objectPalette.objects.getComponent<Components::Sprite>(currentObject);
    if (spriteComp)
    {
        currentObjectSprite = spriteComp->sprite;
        //currentObjectSprite.setTexture(spriteComp->sprite.getTexture());
    }
}

void LevelEditor::resize(int deltaX, int deltaY)
{
    auto mapSize = ng::vec::cast<int>(world.tileMap.getMapSize());
    world.tileMapChanger.resize(mapSize.x + deltaX, mapSize.y + deltaY);
    updateBorder();
}

void LevelEditor::initialize()
{
    // Try to get the object ID for initial "on" states
    stateOnId = world.level.getObjectIdFromName("onStates");
    if (stateOnId == ocs::invalidID)
    {
        // Add TileChanger objects for initial on/off states of tiles
        stateOnId = world.objects.createObject("TileController");

        // Set initial state
        auto comp = world.objects.getComponent<Components::TileGroup>(stateOnId);
        if (comp)
            comp->initialState = true;

        world.level.registerObjectName(stateOnId, "onStates");
    }
}
