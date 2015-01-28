// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MAGICWINDOW_H
#define MAGICWINDOW_H

#include <SFML/Graphics.hpp>

/*
TODO:
Remove dependencies of the magic window class that don't logically make sense.
    This class will need to somehow modify or "rebuild" parts of the level,
    which in turn will affect the collision detection, logic, and graphics.
Fix bug where objects can be in a non-collidable area, which is collidable in the
    alternate world, and the window is moved away or turned off. This causes the collision
    handling to move you away randomly.
    Possible fix: Keep a "trail" stored in some kind of list, and back trace the list until
        it's not colliding with anything. Also, the list would have to be unlimited, but it
        would only record when the object is inside of the window. Then, when the position is
        being "fixed", it could smoothly animate it. This could end up being another game
        mechanic! Although it is sort of like time travel, which might make things too
        complex. It shouldn't be hard to design levels to avoid confusion.
*/

/*
This class contains the in-game window into the alternate world, which is the main game mechanic.
*/
class MagicWindow: public sf::Drawable, public sf::Transformable
{
    public:
        MagicWindow();

        // Processes events
        void update();

        // Setup
        void setTileSize(const sf::Vector2u& newTileSize);
        void setCenter(const sf::Vector2f& center, bool force = false, bool updatePreview = true);
        void setSize(const sf::Vector2f& newSize);
        void setSize(unsigned newBlockSize);

        // Returns true if the position or size changed
        bool hasChanged() const;

        // Used for drawing the alternate level to
        sf::RenderTexture& getTexture();

        // Collision detection
        bool isWithin(const sf::Vector2u& pos) const;
        bool isWithin(const sf::FloatRect& aabb) const;

        // Set/get state
        void show(bool state);
        bool isVisible() const;

        // Graphics
        void setView(const sf::View& view, const sf::Vector2f& windowViewPos);
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:

        void handleResize(int delta);
        void handleKeyPressed(const sf::Event::KeyEvent& keyEvent);

        static const unsigned DEFAULT_BLOCK_SIZE = 5;
        static const unsigned MAX_BLOCK_SIZE = 5;
        static const unsigned MIN_BLOCK_SIZE = 2;

        // States
        bool changed;
        bool visible;
        bool active; // If mouse input should take effect

        // Settings
        unsigned blockSize;
        sf::Vector2u tileSize;
        sf::Vector2f size;
        sf::Vector2f position;
        sf::Vector2f center;

        // Graphics
        sf::RectangleShape border;
        sf::RectangleShape preview;
        sf::RenderTexture texture;
        sf::View textureView; // Stays the size of the texture
};

#endif
