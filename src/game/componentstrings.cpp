// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#include "componentstrings.h"
#include "components.h"
#include "movingcomponent.h"
#include "lasercomponent.h"

void bindComponentStrings(ocs::ObjectManager& entities)
{
    entities.bindStringToComponent<Components::Position>("Position");
    entities.bindStringToComponent<Components::Velocity>("Velocity");
    entities.bindStringToComponent<Components::Gravity>("Gravity");
    entities.bindStringToComponent<Components::Size>("Size");
    entities.bindStringToComponent<Components::AABB>("AABB");
    entities.bindStringToComponent<Components::Sprite>("Sprite");
    entities.bindStringToComponent<Components::AnimSprite>("AnimSprite");
    entities.bindStringToComponent<Components::Jumpable>("Jumpable");
    entities.bindStringToComponent<Components::ObjectState>("ObjectState");
    entities.bindStringToComponent<Components::Movable>("Movable");
    entities.bindStringToComponent<Components::Carrier>("Carrier");
    entities.bindStringToComponent<Components::CameraUpdater>("CameraUpdater");
    entities.bindStringToComponent<Components::AltWorld>("AltWorld");
    entities.bindStringToComponent<Components::Carryable>("Carryable");
    entities.bindStringToComponent<Components::DrawOnTop>("DrawOnTop");
    entities.bindStringToComponent<Components::AboveWindow>("AboveWindow");
    entities.bindStringToComponent<Components::Moving>("Moving");
    entities.bindStringToComponent<Components::State>("State");
    entities.bindStringToComponent<Components::TileGroup>("TileGroup");
    entities.bindStringToComponent<Components::Laser>("Laser");
    entities.bindStringToComponent<Components::TilePosition>("TilePosition");
    entities.bindStringToComponent<Components::Rotation>("Rotation");
}
