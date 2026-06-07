#pragma once

#include "raylib/raylib.h"

#include "Map.h"

typedef struct Player Player;
struct Player {

    Rectangle rect;
    Vector2 vel;
    Color color;

    float walkingSpeed;
    float jumpSpeed;
    float maxFallSpeed;

    int jumpCount;
    int maxJumps;

    bool lookingRight;

    float walkFrameTime;
    float walkFrameCounter;
    int currentWalkFrame;
    int totalWalkFrames;

    Vector2 pickaxePos;
    float pickaxeStartAngle;
    float pickaxeAngle;
    float pickaxeAngleVel;
    bool swingPickaxe;

    void (*input)( Player *player, Map *map, Camera2D *camera );
    void (*update)( Player *player, Map *map, float delta );
    void (*draw)( Player *player );

};

Player *createPlayer( int x, int y, int width, int height, Color color );
void destroyPlayer( Player *player );
