// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "selectiongui.h"
#include "gameinstance.h"
#include "nage/graphics/vectors.h"
#include "es/events.h"
#include "gameevents.h"
#include "components.h"

SelectionGUI::SelectionGUI(GameInstance& gameInstance, sf::RenderWindow& window, es::World& palette):
    gameInstance(gameInstance),
    window(window),
    palette(palette)
{
    // Calculate views and sizes
    view = gameInstance.camera.getView("window");
    tileSize = gameInstance.tileMap.getTileSize();
    borderSize = sf::Vector2f(WIDTH_PIXELS + (PADDING * 2),
                              view.getSize().y - (PADDING * 2));
    float textureHeight = borderSize.y - BUTTON_HEIGHT - (PADDING * 4);
    float viewWidth = tileSize.x * TILE_COLUMNS;
    float viewHeight = (textureHeight / WIDTH_PIXELS) * viewWidth;

    // Load tiles and world
    setupTiles();
    setupObjects();

    // Setup render texture
    textureView.reset(sf::FloatRect(0, 0, viewWidth, viewHeight));
    texture.create(WIDTH_PIXELS, textureHeight);
    texturePos = sf::Vector2f(PADDING * 2, BUTTON_HEIGHT + (PADDING * 4));
    sprite.setPosition(texturePos);
    sprite.setTexture(texture.getTexture());

    // Setup selection box
    currentSelection.setSize(ng::vec::cast<float>(tileSize));
    currentSelection.setFillColor(sf::Color::Transparent);
    currentSelection.setOutlineColor(sf::Color::Red);
    currentSelection.setOutlineThickness(-16);

    // Setup hover selection box
    hoverSelection = currentSelection;
    hoverSelection.setOutlineColor(sf::Color(255, 0, 0, 128));

    // Setup border
    border.setPosition(PADDING, PADDING);
    border.setSize(borderSize);
    border.setFillColor(sf::Color(64, 64, 64, 192));
    border.setOutlineColor(sf::Color(0, 0, 255, 192));
    border.setOutlineThickness(PADDING);

    // Setup tabs
    font.loadFromFile("data/fonts/Ubuntu-B.ttf");
    const sf::Vector2u tabSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    auto& tilesTab = tabs["tiles"];
    auto& objectsTab = tabs["world"];
    tilesTab.setup(font, sf::Vector2f(PADDING * 2, PADDING * 2), tabSize, "Tiles");
    tilesTab.setFontSize(24);
    tilesTab.setPressedCallback([&](ng::Button& b){ handleTilesTab(); });
    objectsTab.setup(font, sf::Vector2f(BUTTON_WIDTH + PADDING * 4, PADDING * 2), tabSize, "Objects");
    objectsTab.setFontSize(24);
    objectsTab.setPressedCallback([&](ng::Button& b){ handleObjectsTab(); });
}

bool SelectionGUI::handleEvent(const sf::Event& event)
{
    bool eventHandled = false;

    // Handle switching tabs
    tabs.handleMouseEvent(event, guiMousePos);

    // Handle selecting tiles/world
    bool clicked = (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);
    bool mouseMoved = (event.type == sf::Event::MouseMoved);
    if (clicked || mouseMoved)
    {
        sf::Vector2i eventMousePos;
        if (clicked)
            eventMousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        else
            eventMousePos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
        eventHandled = handleMouseEvent(eventMousePos, clicked);
    }

    return eventHandled;
}

bool SelectionGUI::update(float dt)
{
    // Update mouse position
    rawMousePos = sf::Mouse::getPosition(window);
    guiMousePos = window.mapPixelToCoords(rawMousePos, view);
    textureMousePos = window.mapPixelToCoords(rawMousePos, textureView);

    // Draw to render texture
    texture.setView(textureView);
    texture.clear(sf::Color::Transparent);

    if (state == TabState::Tiles)
        texture.draw(tiles);
    else if (state == TabState::Objects)
    {
        for (auto& spriteComp: gameInstance.world.getComponents<Sprite>())
            texture.draw(spriteComp.sprite);
    }

    if (state == selState)
        texture.draw(currentSelection);

    if (showHover)
        texture.draw(hoverSelection);

    texture.display();

    sprite.setTexture(texture.getTexture());

    return withinBorder(guiMousePos);
}

void SelectionGUI::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.setView(view);
    target.draw(border);
    target.draw(tabs);
    target.draw(sprite);
}

void SelectionGUI::setupTiles()
{
    // Setup tile map
    tiles.loadFromConfig("data/config/tilemap.cfg");
    unsigned tileTypes = tiles.getTotalTypes();
    unsigned rows = tileTypes / TILE_COLUMNS + 1;
    tiles.resize(TILE_COLUMNS, rows);
    tiles.useLayer(0);

    // Generate tiles
    unsigned id = 0;
    for (unsigned y = 0; y < rows && id < tileTypes; ++y)
    {
        for (unsigned x = 0; x < TILE_COLUMNS && id < tileTypes; ++x)
            tiles.set(x, y, id++);
    }
}

void SelectionGUI::setupObjects()
{
    // Update positions of objects
    sf::Vector2u gridPos;
    for (auto ent: palette.query<Sprite>())
    {
        auto spriteComp = ent.get<Sprite>();

        // Use center-based origin points for positions if there is a Rotation component
        sf::Vector2f offset;
        if (ent.has<Rotation>())
        {
            auto bounds = spriteComp->sprite.getGlobalBounds();
            offset = sf::Vector2f(bounds.width / 2.0f, bounds.height / 2.0f);
        }

        // Set position of sprite
        spriteComp->sprite.setPosition(gridPos.x * tileSize.x + offset.x, gridPos.y * tileSize.y + offset.y);
        std::cout << gridPos.x * tileSize.x << ", " << gridPos.y * tileSize.y << "\n";

        // Go to next position or next line
        ++gridPos.x;
        if (gridPos.x >= TILE_COLUMNS)
        {
            gridPos.x = 0;
            ++gridPos.y;
        }
    }
}

bool SelectionGUI::withinBorder(const sf::Vector2f& pos) const
{
    return border.getGlobalBounds().contains(pos);
}

bool SelectionGUI::withinTexture(const sf::Vector2f& pos) const
{
    return sprite.getGlobalBounds().contains(pos);
}

void SelectionGUI::handleTilesTab()
{
    state = TabState::Tiles;
}

void SelectionGUI::handleObjectsTab()
{
    state = TabState::Objects;
}

bool SelectionGUI::handleMouseEvent(const sf::Vector2i& pos, bool clicked)
{
    bool handled = false;
    auto eventMousePos = pos;
    auto mousePosGui = window.mapPixelToCoords(eventMousePos, view);

    // Adjust mouse position for texture
    eventMousePos.x -= texturePos.x;
    eventMousePos.y -= texturePos.y;
    auto mousePosTex = texture.mapPixelToCoords(eventMousePos);

    showHover = false;
    if (withinTexture(mousePosGui))
    {
        select(mousePosTex, clicked);
        handled = true;
    }
    else if (withinBorder(mousePosGui))
        handled = true;

    return handled;
}

void SelectionGUI::select(const sf::Vector2f& pos, bool clicked)
{
    sf::Vector2u tilePos(pos.x / tileSize.x, pos.y / tileSize.y);
    if (state == TabState::Tiles)
    {
        if (tiles.inBounds(tilePos.x, tilePos.y))
        {
            //std::cout << "tilePos = (" << tilePos.x << ", " << tilePos.y << ")\n";

            // Calculate position to move selection box
            sf::Vector2f selectionPos(tilePos.x * tileSize.x, tilePos.y * tileSize.y);
            hoverSelection.setPosition(selectionPos);
            showHover = true;

            if (clicked)
            {
                // Update current selection
                currentSelection.setPosition(selectionPos);
                selState = TabState::Tiles;

                // Send event with new visual tile ID
                int visualId = tiles(tilePos.x, tilePos.y);
                es::Events::send(TileSelectionEvent{visualId});
            }
        }
    }
    else if (state == TabState::Objects)
    {
        hoverSelection.setPosition(pos);
        showHover = true;

        if (clicked)
        {
            auto ent = checkCollision(palette, pos);
            if (ent)
            {
                auto spriteComp = ent.get<Sprite>();
                if (spriteComp)
                {
                    // Update current selection
                    auto position = spriteComp->sprite.getPosition();
                    auto origin = spriteComp->sprite.getOrigin();
                    currentSelection.setPosition(position.x - origin.x, position.y - origin.y);
                    selState = TabState::Objects;

                    // Send event with selected object ID
                    es::Events::send(ObjectSelectionEvent{ent.getId()});
                }
            }
        }
    }
}

es::Entity SelectionGUI::checkCollision(es::World& world, const sf::Vector2f& mousePos)
{
    for (auto ent: world.query<Sprite>())
    {
        auto bounds = ent.get<Sprite>()->sprite.getGlobalBounds();
        if (bounds.contains(mousePos))
            return ent;
    }
    return {world};
}
