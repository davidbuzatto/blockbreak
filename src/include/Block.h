#pragma once

#include <stdbool.h>

#include "raylib/raylib.h"

#include "Typedefs.h"

struct Block {
    Rectangle rect;
    Color color;
    bool broken;
};
