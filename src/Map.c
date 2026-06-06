#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"
#include "stb/stb_perlin.h"

#include "Typedefs.h"
#include "Block.h"
#include "Map.h"

static void draw( Map *map );
static void drawBlock( Block *block );

Map *createMap( int x, int y, int lines, int columns, int blockSize ) {

    Map *new = (Map*) malloc( sizeof( Map ) );

    new->lines = lines;
    new->columns = columns;

    new->blockSize = blockSize;
    new->blocks = (Block*) malloc( sizeof( Block ) * lines * columns );

    float scale = 0.1f;

    for ( int i = 0; i < new->lines; i++ ) {
        for ( int j = 0; j < new->columns; j++ ) {

            float nx = j * scale;
            float ny = i * scale;
            float n = stb_perlin_noise3( nx, ny, 0, 0, 0, 0 );

            Color color;
            int hitsToBreak;
            bool broken = false;

            if ( n < -0.3f ) {
                color = BROWN;
                hitsToBreak = 1;
            } else if ( n < 0.1f ) {
                color = GREEN;
                hitsToBreak = 1;
                broken = true;
            } else if ( n < 0.5f ) {
                color = GRAY;
                hitsToBreak = 2;
            } else {
                color = DARKGRAY;
                hitsToBreak = 3;
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
    return map->lines * map->blockSize;
}

static void draw( Map *map ) {
    for ( int i = 0; i < map->lines; i++ ) {
        for ( int j = 0; j < map->columns; j++ ) {
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