#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"
#include "stb/stb_perlin.h"

#include "Block.h"
#include "BlockRange.h"
#include "Map.h"
#include "ResourceManager.h"
#include "Utils.h"

static void fillMap( Map *map, float scale, float seed );
static void draw( Map *map, Camera2D *camera );
static void drawBlock( Block *block );

Map *createMap( int x, int y, int rows, int columns, int blockSize ) {

    Map *new = (Map*) malloc( sizeof( Map ) );

    new->pos.x = x;
    new->pos.y = y;

    new->rows = rows;
    new->columns = columns;

    new->blockSize = blockSize;
    new->blocks = (Block*) malloc( sizeof( Block ) * new->rows * new->columns );

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
    return map->rows * map->blockSize;
}

static void fillMap( Map *map, float scale, float seed ) {

    for ( int i = 0; i < map->rows; i++ ) {
        for ( int j = 0; j < map->columns; j++ ) {

            float nx = j * scale;
            float ny = i * scale;
            float n = stb_perlin_noise3( nx, ny, seed, 0, 0, 0 );

            Color color;
            int atlasIndex = 0;
            int hitsToBreak = 1;
            bool broken = false;
                        
            if ( n < -0.60f ) {        // diamond
                color = WHITE;
                atlasIndex = 15;
                hitsToBreak = 10;
            } else if ( n < -0.40f ) { // gold
                color = GOLD;
                atlasIndex = 16;
                hitsToBreak = 4;
            } else if ( n < -0.10f ) { // rock
                color = DARKGRAY;
                atlasIndex = 7;
                hitsToBreak = 6;
            } else if ( n < 0.30f ) {  // empty
                color = WHITE;
                broken = true;
            } else if ( n < 0.45f ) {  // mud
                color = BROWN;
                atlasIndex = 6;
                hitsToBreak = 2;
            } else if ( n < 0.70f ) {  // dirt
                color = DARKBROWN;
                atlasIndex = 4;
                hitsToBreak = 2;
            } else {                   // emerald
                color = LIME;
                atlasIndex = 18;
                hitsToBreak = 7;
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
                .broken = broken
            };

        }
    }

    // redo the first line to generate grass
    for ( int i = 0; i < 2; i++ ) {
        for ( int j = 0; j < map->columns; j++ ) {
            int p = i * map->columns + j;
            map->blocks[p].color = GREEN;
            map->blocks[p].atlasIndex = i == 0 ? 1 : 0;
            map->blocks[p].hitsToBreak = 2;
            map->blocks[p].broken = false;
        }
    }

}

static void draw( Map *map, Camera2D *camera ) {

    BlockRange range = getVisibleBlocks( map, camera );

    DrawRectangle( 
        map->pos.x,
        map->pos.y,
        calcMapWidth( map ),
        calcMapHeight( map ),
        DARKBROWN
    );

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