#pragma once

#include "raylib/raylib.h"

#include "Typedefs.h"

struct Player {

    Rectangle rect;
    Vector2 vel;
    Color color;

    float walkingSpeed;
    float jumpSpeed;
    float maxFallSpeed;

    int jumpCount;
    int maxJumps;

    void (*input)( Player *player, Map *map, Camera2D *camera );
    void (*update)( Player *player, Map *map, float delta );
    void (*draw)( Player *player );

};

Player *createPlayer( int x, int y, int width, int height, Color color );
void destroyPlayer( Player *player );
