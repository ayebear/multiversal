// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef GAMEMENU_H
#define GAMEMENU_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>
#include "configfile.h"
#include "colorcode.h"
#include "spriteloader.h"

/*
An SFML menu designed for use with games.
You can configure various settings in the config file.
Buttons can have different settings for each state.
*/
class GameMenu: public sf::Drawable
{
    public:

        using CallbackType = std::function<void()>;

        // Setup
        GameMenu(sf::RenderWindow& window, const std::string& configFilename);
        void addItem(const std::string& name, CallbackType callback);

        // These should be called while running
        void handleEvent(const sf::Event& event);
        void update(float dt);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:

        int getSelectedItem() const;
        void mapMousePos(const sf::Vector2i& pos);
        void selectMenuItem(int index);
        void loadSettings();

        struct MenuItem
        {
            MenuItem(const std::string& name, CallbackType callback);
            sf::FloatRect rect;
            sf::RectangleShape shape;
            sf::Text label;
            std::string name;
            CallbackType callback;
        };

        struct ButtonSettings
        {
            unsigned fontSize;
            unsigned outlineThickness;
            ColorCode colorFill;
            ColorCode colorOutline;
            ColorCode fontColor;
        };

        static const int NO_SELECTION = -1;

        // Main objects/variables
        sf::RenderWindow& window;
        cfg::File config;
        int currentItem;
        std::vector<MenuItem> menuItems;
        sf::Vector2f mousePos;
        sf::View view;
        sf::Vector2f viewSize;
        sf::Sprite backgroundSprite;

        // Settings
        float transitionTime;
        int padding;
        int textPaddingTop;
        unsigned width;
        unsigned height;
        unsigned fontSize;
        sf::Font font;
        ButtonSettings buttonSettings[2];

        // Calculated settings
        sf::Vector2f firstButton;
};

#endif
