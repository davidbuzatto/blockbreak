#include "raylib/raylib.h"

#include "BlockRange.h"
#include "Map.h"
#include "Utils.h"

static BlockRange clampRange( BlockRange range, Map *map );

BlockRange getNeighborBlocks( Map *map, Rectangle rect ) {

    BlockRange range = {
        .rowMin = (int) ( ( rect.y - map->pos.y ) / map->blockSize ) - 1,
        .rowMax = (int) ( ( rect.y + rect.height - map->pos.y ) / map->blockSize ) + 1,
        .colMin = (int) ( ( rect.x - map->pos.x ) / map->blockSize ) - 1,
        .colMax = (int) ( ( rect.x + rect.width  - map->pos.x ) / map->blockSize ) + 1,
    };

    return clampRange( range, map );

}

BlockRange getVisibleBlocks( Map *map, Camera2D *camera ) {

    float worldLeft = camera->target.x - camera->offset.x / camera->zoom;
    float worldTop = camera->target.y - camera->offset.y / camera->zoom;
    float worldRight  = camera->target.x + ( GetScreenWidth() - camera->offset.x ) / camera->zoom;
    float worldBottom = camera->target.y + ( GetScreenHeight() - camera->offset.y ) / camera->zoom;

    BlockRange range = {
        .rowMin = (int) ( ( worldTop - map->pos.y ) / map->blockSize ),
        .rowMax = (int) ( ( worldBottom - map->pos.y ) / map->blockSize ) + 1,
        .colMin = (int) ( ( worldLeft - map->pos.x ) / map->blockSize ),
        .colMax = (int) ( ( worldRight - map->pos.x ) / map->blockSize ) + 1,
    };

    return clampRange( range, map );

}

static BlockRange clampRange( BlockRange range, Map *map ) {

    if ( range.rowMin < 0 ) range.rowMin = 0;
    if ( range.colMin < 0 ) range.colMin = 0;
    if ( range.rowMax >= map->surfRows + map->subtRows ) range.rowMax = map->surfRows + map->subtRows - 1;
    if ( range.colMax >= map->columns ) range.colMax = map->columns - 1;

    return range;

}
