#pragma once

#include "raylib/raylib.h"

#include "Typedefs.h"

struct Map {

    int lines;
    int columns;

    int blockSize;
    Block *blocks;

    void (*draw)( Map *map );

};

Map *createMap( int x, int y, int lines, int columns, int blockSize );
void destroyMap( Map *map );

int calcMapWidth( Map *map );
int calcMapHeight( Map *map );
