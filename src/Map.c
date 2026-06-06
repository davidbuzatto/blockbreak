#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"

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

    for ( int i = 0; i < new->lines; i++ ) {
        for ( int j = 0; j < new->columns; j++ ) {
            int p = i * new->columns + j;
            new->blocks[p] = (Block) {
                .rect = { 
                    x + new->blockSize * j, 
                    y + new->blockSize * i,
                    new->blockSize,
                    new->blockSize
                },
                .color = ORANGE,
                .broken = false
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
        DrawRectangleLinesEx( block->rect, 1.0f, BLACK );
    }
}