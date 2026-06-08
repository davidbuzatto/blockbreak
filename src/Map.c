#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"
#include "stb/stb_perlin.h"

#include "Block.h"
#include "BlockRange.h"
#include "Macros.h"
#include "Map.h"
#include "ResourceManager.h"
#include "Utils.h"

static void fillMap( Map *map, float scale, float seed );
static void draw( Map *map, Camera2D *camera );
static void drawBlock( Block *block );

Map *createMap( int x, int y, int surfRows, int subtRows, int columns, int blockSize ) {

    Map *new = (Map*) malloc( sizeof( Map ) );

    new->pos.x = x;
    new->pos.y = y;

    new->surfRows = surfRows;
    new->subtRows = subtRows;
    new->columns = columns;

    new->blockSize = blockSize;
    new->blocks = (Block*) malloc( sizeof( Block ) * ( new->surfRows + new->subtRows ) * new->columns );

    //fillMap( new, 0.1f, 0 );
    fillMap( new, 0.1f, GetRandomValue( 0, 10000 ) );

    new->draw = draw;

    return new;

}

void destroyMap( Map *map ) {
    if ( map != NULL ) {
        free( map->blocks );
        free( map );
    }
}

int calcMapWidth( Map *map ) {
    return map->columns * map->blockSize;
}

int calcMapHeight( Map *map ) {
    return ( map->surfRows + map->subtRows ) * map->blockSize;
}

int calcMapSurfHeight( Map *map ) {
    return map->surfRows * map->blockSize;
}

int calcMapSubtHeight( Map *map ) {
    return map->subtRows * map->blockSize;
}

static void fillMap( Map *map, float scale, float seed ) {

    for ( int i = 0; i < map->surfRows; i++ ) {
        for ( int j = 0; j < map->columns; j++ ) {
            int p = i * map->columns + j;
            map->blocks[p] = (Block) {
                .rect = { 
                    map->pos.x + map->blockSize * j, 
                    map->pos.y + map->blockSize * i,
                    map->blockSize,
                    map->blockSize
                },
                .color = WHITE,
                .atlasIndex = 0,
                .hitsToBreak = 0,
                .hits = 0,
                .materialsToAquire = 0,
                .broken = true
            };
        }
    }

    for ( int i = map->surfRows; i < map->surfRows + map->subtRows; i++ ) {
        for ( int j = 0; j < map->columns; j++ ) {

            float nx = j * scale;
            float ny = i * scale;
            float n = stb_perlin_noise3( nx, ny, seed, 0, 0, 0 );

            Color color;
            int atlasIndex = 0;
            int hitsToBreak = 1;
            int materialsToAquire = 0;
            bool broken = false;
                        
            if ( n < -0.60f ) {        // diamond
                color = WHITE;
                atlasIndex = 15;
                hitsToBreak = 10;
                materialsToAquire = 100;
            } else if ( n < -0.40f ) { // gold
                color = GOLD;
                atlasIndex = 16;
                hitsToBreak = 4;
                materialsToAquire = 50;
            } else if ( n < -0.10f ) { // rock
                color = DARKGRAY;
                atlasIndex = 7;
                hitsToBreak = 6;
                materialsToAquire = 20;
            } else if ( n < 0.30f ) {  // empty
                color = WHITE;
                broken = true;
            } else if ( n < 0.45f ) {  // mud
                color = BROWN;
                atlasIndex = 6;
                hitsToBreak = 2;
                materialsToAquire = 10;
            } else if ( n < 0.70f ) {  // dirt
                color = DARKBROWN;
                atlasIndex = 4;
                hitsToBreak = 2;
                materialsToAquire = 10;
            } else {                   // emerald
                color = LIME;
                atlasIndex = 18;
                hitsToBreak = 7;
                materialsToAquire = 80;
            }

            int p = i * map->columns + j;
            map->blocks[p] = (Block) {
                .rect = { 
                    map->pos.x + map->blockSize * j, 
                    map->pos.y + map->blockSize * i,
                    map->blockSize,
                    map->blockSize
                },
                .color = color,
                .atlasIndex = atlasIndex,
                .hitsToBreak = hitsToBreak,
                .hits = 0,
                .materialsToAquire = materialsToAquire,
                .broken = broken
            };

        }
    }

    // redo the first two underground rows to generate grass transition
    for ( int i = map->surfRows; i < map->surfRows + 2; i++ ) {
        for ( int j = 0; j < map->columns; j++ ) {
            int p = i * map->columns + j;
            map->blocks[p].color = GREEN;
            map->blocks[p].atlasIndex = i == map->surfRows ? 1 : 0;
            map->blocks[p].hitsToBreak = 2;
            map->blocks[p].materialsToAquire = 1;
            map->blocks[p].broken = false;
        }
    }

}

static void draw( Map *map, Camera2D *camera ) {

    BlockRange range = getVisibleBlocks( map, camera );

    /*DrawRectangle( 
        map->pos.x,
        map->surfRows * map->blockSize,
        calcMapWidth( map ),
        calcMapHeight( map ),
        DARKBROWN
    );*/

    int horReapeats = calcMapWidth( map ) / rm.skyBgTexture.width;
    int verReapeats = ( map->subtRows * map->blockSize ) / rm.caveBgTexture.height;

    float horParallaxPerc = ( (float) camera->target.x / calcMapWidth( map ) );
    float verParallaxPerc = ( (float) camera->target.y / calcMapHeight( map ) );
    float horParallaxSkyDisp = horParallaxPerc * rm.skyBgTexture.width;
    float horParallaxCaveDisp = horParallaxPerc * rm.skyBgTexture.width * 0.75f;
    float verParallaxCaveDisp = verParallaxPerc * rm.caveBgTexture.height * 0.75f;

    for ( int i = -1; i <= verReapeats; i++ ) {
        for ( int j = -1; j <= horReapeats; j++ ) {
            DrawTexture(
                rm.caveBgTexture, 
                map->pos.x + rm.caveBgTexture.width * j + horParallaxCaveDisp, 
                map->surfRows * map->blockSize + ( rm.caveBgTexture.height * i ) + verParallaxCaveDisp, 
                WHITE
            );
        }
    }

    for ( int i = -1; i <= horReapeats; i++ ) {
        DrawTexture(
            rm.skyBgTexture, 
            map->pos.x + rm.skyBgTexture.width * i + horParallaxSkyDisp, 
            map->surfRows * map->blockSize - rm.skyBgTexture.height, 
            WHITE
        );
    }

    for ( int i = range.rowMin; i <= range.rowMax; i++ ) {
        for ( int j = range.colMin; j <= range.colMax; j++ ) {
            int p = i * map->columns + j;
            Block *b = &map->blocks[p];
            drawBlock( b );
        }
    }
}

static void drawBlock( Block *block ) {

    if ( !block->broken ) {

        //DrawRectangleRec( block->rect, block->color );
        DrawTexturePro( 
            rm.terrainsTexture,
            (Rectangle) { 0, ( 20 + 2 ) * block->atlasIndex, 20, 20 },
            block->rect,
            (Vector2) { 0 },
            0.0f,
            WHITE
        );

        int hitPercentage = (int) ( (float) block->hits / block->hitsToBreak * 100 );

        int crackRow = 1;
        int crackColumn = 0;
        bool drawCrack = true;

        if ( hitPercentage < 25 ) {
            drawCrack = false;
        } else if ( hitPercentage >= 25 && hitPercentage < 50 ) {
            crackColumn = 0;
        } else if ( hitPercentage >= 50 && hitPercentage < 75 ) {
            crackColumn = 1;
        } else if ( hitPercentage >= 75 ) {
            crackColumn = 2;
        }

        if ( drawCrack ) {
            DrawTexturePro( 
                rm.cracksTexture,
                (Rectangle) { 22 * crackColumn, 22 * crackRow, 20, 20 },
                block->rect,
                (Vector2) { 0 },
                0.0f,
                WHITE
            );
        }

        /*DrawText( 
            TextFormat( "%d", block->hitsToBreak ), 
            block->rect.x + block->rect.width / 2 - 2,
            block->rect.y + block->rect.height / 2 - 8,
            20,
            ColorBrightness( block->color, -0.5f )
        );*/
        //DrawRectangleLinesEx( block->rect, 1.0f, BLACK );

    }
}