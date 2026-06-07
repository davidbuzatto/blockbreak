#pragma once

#include "raylib/raylib.h"

#include "BlockRange.h"
#include "Map.h"

BlockRange getNeighborBlocks( Map *map, Rectangle rect );
BlockRange getVisibleBlocks( Map *map, Camera2D *camera );