#pragma once

#include "raylib/raylib.h"

#include "Block.h"

typedef struct Map Map;
struct Map {

    Vector2 pos;

    int rows;
    int columns;

    int blockSize;
    Block *blocks;

    void (*draw)( Map *map, Camera2D *camera );

};

Map *createMap( int x, int y, int rows, int columns, int blockSize );
void destroyMap( Map *map );

int calcMapWidth( Map *map );
int calcMapHeight( Map *map );
