// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "gamemenu.h"
#include "vectors.h"
#include <iostream>

GameMenu::GameMenu(sf::RenderWindow& window, const std::string& configFilename):
    window(window),
    config(configFilename),
    currentItem(NO_SELECTION),
    mouseMoved(false)
{
    view = window.getDefaultView();
    viewSize = view.getSize();
    loadActions();
    loadSettings();
}

void GameMenu::addItem(const std::string& name, CallbackType callback)
{
    menuItems.emplace_back(name, callback);
    menuItems.back().label.setFont(font);
}

void GameMenu::handleEvent(const sf::Event& event)
{
    actions.handleEvent(event);

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        // Handle clicking on a menu item
        mapMousePos(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        selectMenuItem(getSelectedItem());
    }
    else if (event.type == sf::Event::MouseMoved)
        mapMousePos(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
}

void GameMenu::update(float dt)
{
    // Check mouse collisions
    if (mouseMoved)
    {
        int found = getSelectedItem();
        if (found != NO_SELECTION)
            currentItem = found;
    }
    mouseMoved = false;

    // Update animations/graphics, positions, sizes, colors, and labels
    int index = 0;
    for (auto& item: menuItems)
    {
        // Check if this button is hovered over
        bool hovered = (index == currentItem);
        auto& settings = buttonSettings[hovered];

        // Update delta times for current animation
        float ratio = updateDt(item.dt, dt, transitionTime, hovered);
        float textRatio = updateDt(item.dtText, dt, textTransitionTime, hovered);

        // Update properties of the button
        // Update logical bounding box
        float yPos = firstButton.y + (index * (height + padding));
        item.rect = sf::FloatRect(firstButton.x, yPos, width, height);

        // Update rectangle shape
        item.shape.setPosition(item.rect.left, item.rect.top);
        item.shape.setSize(sf::Vector2f(width, height));
        item.shape.setOutlineThickness(interpolate(buttonSettings[0].outlineThickness, buttonSettings[1].outlineThickness, ratio));
        item.shape.setFillColor(averageColors(buttonSettings[0].colorFill.toColor(), buttonSettings[1].colorFill.toColor(), ratio));
        item.shape.setOutlineColor(averageColors(buttonSettings[0].colorOutline.toColor(), buttonSettings[1].colorOutline.toColor(), ratio));

        // Update text
        item.label.setCharacterSize(fontSize);
        item.label.setColor(averageColors(buttonSettings[0].fontColor.toColor(), buttonSettings[1].fontColor.toColor(), textRatio));

        // Calculate offset and update text position
        auto textBounds = item.label.getGlobalBounds();
        sf::Vector2f textOffset((width - textBounds.width) / 2, textPaddingTop);
        item.label.setPosition(item.rect.left + textOffset.x, item.rect.top + textOffset.y);

        ++index;
    }
}

void GameMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(backgroundSprite);
    target.draw(foregroundSprite);
    for (auto& item: menuItems)
    {
        target.draw(item.shape);
        target.draw(item.label);
    }
}

int GameMenu::getSelectedItem() const
{
    // See if the mouse is hovering over any menu items
    int index = 0;
    int found = NO_SELECTION;
    for (auto& item: menuItems)
    {
        if (item.rect.contains(mousePos))
            found = index;
        ++index;
    }
    return found;
}

void GameMenu::mapMousePos(const sf::Vector2i& pos)
{
    mousePos = window.mapPixelToCoords(pos);
    mouseMoved = true;
}

void GameMenu::selectMenuItem(int index)
{
    currentItem = index;
    // Execute callback of menu item
    if (index != NO_SELECTION)
        menuItems[index].callback();
}

void GameMenu::loadActions()
{
    actions.loadSection(config.getSection("Controls"));
    actions["select"].setCallback([&](){selectMenuItem(currentItem);});
    actions["moveUp"].setCallback(std::bind(&GameMenu::moveUp, this));
    actions["moveDown"].setCallback(std::bind(&GameMenu::moveDown, this));
}

void GameMenu::loadSettings()
{
    // Load general settings
    SpriteLoader::load(backgroundSprite, config("backgroundImage"), true);
    SpriteLoader::load(foregroundSprite, config("foregroundImage"), true);
    transitionTime = config("transitionTime").toFloat();
    textTransitionTime = config("textTransitionTime").toFloat();
    padding = config("padding").toInt();
    textPaddingTop = config("textPaddingTop").toInt();
    font.loadFromFile(config("fontFile"));
    width = config("buttonWidth").toInt();
    height = config("buttonHeight").toInt();
    fontSize = config("fontSize").toInt();

    // Load button settings
    unsigned index = 0;
    for (auto& section: {"Button", "Button.OnHover"})
    {
        auto& settings = buttonSettings[index++];
        config.useSection(section);
        settings.outlineThickness = config("outlineThickness").toInt();
        settings.colorFill = config("colorFill");
        settings.colorOutline = config("colorOutline");
        settings.fontColor = config("fontColor");
    }

    // Calculate positions and text padding
    config.useSection();
    firstButton.x = (viewSize.x - width) / 2;
    firstButton.y = config("firstButtonOffset").toInt();

    // Center foreground sprite
    int foregroundWidth = foregroundSprite.getTexture()->getSize().x;
    foregroundSprite.setPosition((viewSize.x - foregroundWidth) / 2, 0);

    // Center and scale background sprite
    auto backgroundSize = vectors::cast<int>(backgroundSprite.getTexture()->getSize());
    backgroundSprite.setPosition((viewSize.x - backgroundSize.x) / 2, (viewSize.y - backgroundSize.y) / 2);
}

sf::Color GameMenu::averageColors(const sf::Color& startColor, const sf::Color& endColor, float ratio) const
{
    return sf::Color(interpolate(startColor.r, endColor.r, ratio),
                     interpolate(startColor.g, endColor.g, ratio),
                     interpolate(startColor.b, endColor.b, ratio),
                     interpolate(startColor.a, endColor.a, ratio));
}

float GameMenu::interpolate(float start, float end, float ratio) const
{
    // Note: Ratio should be between 0 and 1
    float change = (start - end) * ratio;
    return (start - change);
}

void GameMenu::moveUp()
{
    if (currentItem > 0)
        --currentItem;
    else if (currentItem == -1)
        currentItem = 0;
}

void GameMenu::moveDown()
{
    if (currentItem < static_cast<int>(menuItems.size()) - 1)
        ++currentItem;
}

float GameMenu::updateDt(float& itemDt, float dt, float animTime, bool hovered)
{
    // If hovered, add to the dt
    // Otherwise, subtract from it
    if (hovered)
        itemDt += dt;
    else
        itemDt -= dt;

    // Keep the dt value in bounds of the animation
    if (itemDt < 0)
        itemDt = 0;
    if (itemDt > animTime)
        itemDt = animTime;

    return safeDivide(itemDt, animTime, float(hovered));
}

GameMenu::MenuItem::MenuItem(const std::string& name, CallbackType callback):
    name(name),
    callback(callback),
    dt(0),
    dtText(0)
{
    label.setString(name);
}
