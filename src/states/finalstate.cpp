// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "finalstate.h"
#include "gameresources.h"
#include <iostream>

FinalState::FinalState(GameResources& resources):
    resources(resources)
{
}

void FinalState::handleEvents()
{
    sf::Event event;
    while (resources.window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            stateEvent.command = ng::StateEvent::Exit;
    }
}

void FinalState::update()
{
    std::cout << "\n=================================================\n";
    std::cout << "CONGRATULATIONS! YOU COMPLETED ALL OF THE LEVELS!\n";
    std::cout << "=================================================\n\n";

    // Temporarily disable this state until it is fully implemented
    stateEvent.command = ng::StateEvent::Pop;
}

void FinalState::draw()
{
}
