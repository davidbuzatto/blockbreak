#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"
#include "stb/stb_perlin.h"

#include "Block.h"
#include "BlockRange.h"
#include "Map.h"
#include "Utils.h"

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

    float scale = 0.1f;

    for ( int i = 0; i < new->rows; i++ ) {
        for ( int j = 0; j < new->columns; j++ ) {

            float nx = j * scale;
            float ny = i * scale;
            float n = stb_perlin_noise3( nx, ny, 0, 0, 0, 0 );

            Color color;
            int hitsToBreak = 1;
            bool broken = false;
                        
            if ( n < -0.60f ) {        // diamond
                color = WHITE;
                hitsToBreak = 9;
            } else if ( n < -0.40f ) { // gold
                color = GOLD;
                hitsToBreak = 2;
            } else if ( n < -0.10f ) { // rock
                color = DARKGRAY;
                hitsToBreak = 6;
            } else if ( n < 0.30f ) {  // empty
                color = GRAY;
                broken = true;
            } else if ( n < 0.45f ) {  // mud
                color = BROWN;
                hitsToBreak = 2;
            } else if ( n < 0.70f ) {  // dirt
                color = DARKBROWN;
                hitsToBreak = 4;
            } else {                   // emerald
                color = LIME;
                hitsToBreak = 7;
            }

            int p = i * new->columns + j;
            new->blocks[p] = (Block) {
                .rect = { 
                    x + new->blockSize * j, 
                    y + new->blockSize * i,
                    new->blockSize,
                    new->blockSize
                },
                .color = color,
                .hitsToBreak = hitsToBreak,
                .broken = broken
            };

        }
    }

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

static void draw( Map *map, Camera2D *camera ) {

    BlockRange range = getVisibleBlocks( map, camera );

    DrawRectangle( 
        map->pos.x,
        map->pos.y,
        calcMapWidth( map ),
        calcMapHeight( map ),
        BEIGE
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
        DrawRectangleRec( block->rect, block->color );
        DrawText( 
            TextFormat( "%d", block->hitsToBreak ), 
            block->rect.x + block->rect.width / 2 - 2,
            block->rect.y + block->rect.height / 2 - 8,
            20,
            ColorBrightness( block->color, -0.5f )
        );
        //DrawRectangleLinesEx( block->rect, 1.0f, BLACK );
    }
}