// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "componentstrings.h"
#include "components.h"

void bindComponentStrings(ocs::ObjectManager& entities)
{
    entities.bindStringToComponent<Components::Position>("Position");
    entities.bindStringToComponent<Components::Velocity>("Velocity");
    entities.bindStringToComponent<Components::Size>("Size");
    entities.bindStringToComponent<Components::AABB>("AABB");
    entities.bindStringToComponent<Components::Sprite>("Sprite");
    entities.bindStringToComponent<Components::AnimSprite>("AnimSprite");
    entities.bindStringToComponent<Components::PlayerState>("PlayerState");
    entities.bindStringToComponent<Components::Carrier>("Carrier");
    entities.bindStringToComponent<Components::Input>("Input");
    entities.bindStringToComponent<Components::CameraUpdater>("CameraUpdater");
    entities.bindStringToComponent<Components::AltWorld>("AltWorld");
    entities.bindStringToComponent<Components::Carryable>("Carryable");
    entities.bindStringToComponent<Components::DrawOnTop>("DrawOnTop");
}
