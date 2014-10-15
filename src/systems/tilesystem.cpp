#include "tilesystem.h"
#include "tilemapdata.h"
#include "events.h"
#include "gameevents.h"
#include "logicaltiles.h"
#include "components.h"
#include <iostream>

TileSystem::TileSystem(ocs::ObjectManager& entities, TileMapData& tileMapData):
    entities(entities),
    tileMapData(tileMapData)
{
}

void TileSystem::update(float dt)
{
    // Handle action key events (when the player presses "up") on different tiles
    Events::clear<LoadNextLevelEvent>();
    for (auto& event: Events::get<ActionKeyEvent>())
    {
        auto aabb = entities.getComponent<Components::AABB>(event.entityId);
        if (aabb)
        {
            for (auto& location: aabb->tileCollisions)
            {
                auto tileId = tileMapData(location.x, location.y).logicalId;
                if (tileId == Tiles::Exit)
                    handleExitTile();
            }
        }
    }
}

void TileSystem::handleExitTile()
{
    if (!Events::exists<LoadNextLevelEvent>())
    {
        std::cout << "Sent LoadNextLevelEvent\n";
        Events::send(LoadNextLevelEvent());
    }

}
