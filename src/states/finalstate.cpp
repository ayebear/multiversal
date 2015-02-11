// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "finalstate.h"
#include "gameobjects.h"
#include <iostream>

FinalState::FinalState(GameObjects& objects):
    objects(objects)
{
}

void FinalState::handleEvents()
{
    sf::Event event;
    while (objects.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = StateEvent::Exit;
    }
}

void FinalState::update()
{
    std::cout << "\n=================================================\n";
    std::cout << "CONGRATULATIONS! YOU COMPLETED ALL OF THE LEVELS!\n";
    std::cout << "=================================================\n\n";

    // Temporarily disable this state until it is fully implemented
    stateEvent.command = StateEvent::Pop;
}

void FinalState::draw()
{
}
