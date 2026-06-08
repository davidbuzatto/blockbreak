#pragma once

#include <stdbool.h>

#include "raylib/raylib.h"

typedef struct Block {

    Rectangle rect;
    Color color;
    int atlasIndex;

    int hitsToBreak;
    int hits;

    int materialsToAquire;

    bool broken;

} Block;
