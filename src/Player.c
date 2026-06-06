#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Typedefs.h"
#include "Block.h"
#include "Map.h"
#include "Player.h"

static void update( Player *player, Map *map, float delta );
static void draw( Player *player );

static void resolveCollisionMapX( Player *player, Map *map );
static void resolveCollisionMapY( Player *player, Map *map );

static void input( Player *player, Map *map, Camera2D *camera );

Player *createPlayer( int x, int y, int width, int height, Color color ) {

    Player *new = (Player*) malloc( sizeof( Player ) );

    new->rect.x = x;
    new->rect.y = y;
    new->rect.width = width;
    new->rect.height = height;
    new->vel = (Vector2) { 0 };
    new->color = color;

    new->walkingSpeed = 200;
    new->jumpSpeed = -300;
    new->maxFallSpeed = 600;

    new->maxJumps = 2;
    new->jumpCount = new->maxJumps; // prevents jumps during startup

    new->input = input;
    new->update = update;
    new->draw = draw;

    return new;

}

void destroyPlayer( Player *player ) {
    if ( player != NULL ) {
        free( player );
    }
}

static void update( Player *player, Map *map, float delta ) {

    player->rect.x += player->vel.x * delta;
    resolveCollisionMapX( player, map );

    player->rect.y += player->vel.y * delta;
    player->vel.y += GRAVITY * delta;
    if ( player->vel.y > player->maxFallSpeed ) {
        player->vel.y = player->maxFallSpeed;
    }
    resolveCollisionMapY( player, map );

}

static void draw( Player *player ) {
    DrawRectangleRec( player->rect, player->color );
}

static void resolveCollisionMapX( Player *player, Map *map ) {

    for ( int i = 0; i < map->lines; i++ ) {
        for ( int j = 0; j < map->columns; j++ ) {
            int p = i * map->columns + j;
            Block *b = &map->blocks[p];
            if ( !b->broken ) {
                if ( CheckCollisionRecs( player->rect, b->rect ) ) {
                    if ( player->vel.x > 0.0f ) {
                        player->rect.x = b->rect.x - player->rect.width;
                    } else if ( player->vel.x < 0.0f ) {
                        player->rect.x = b->rect.x + b->rect.width;
                    } else {
                        // untie for vel.x = 0
                        float playerCenterX = player->rect.x + player->rect.width / 2;
                        float blockCenterX = b->rect.x + b->rect.width / 2;
                        if ( playerCenterX < blockCenterX ) {
                            player->rect.x = b->rect.x - player->rect.width;
                        } else {
                            player->rect.x = b->rect.x + b->rect.width;
                        }
                    }
                    player->vel.x = 0;
                }
            }
        }
    }

}

static void resolveCollisionMapY( Player *player, Map *map ) {

    for ( int i = 0; i < map->lines; i++ ) {
        for ( int j = 0; j < map->columns; j++ ) {
            int p = i * map->columns + j;
            Block *b = &map->blocks[p];
            if ( !b->broken ) {
                if ( CheckCollisionRecs( player->rect, b->rect ) ) {
                    if ( player->vel.y > 0.0f ) {
                        player->rect.y = b->rect.y - player->rect.height;
                        player->jumpCount = 0;
                    } else {
                        player->rect.y = b->rect.y + b->rect.height;
                    }
                    player->vel.y = 0;
                }
            }
        }
    }

}

static void input( Player *player, Map *map, Camera2D *camera ) {

    int left = IsKeyDown( KEY_LEFT ) ? -1 : 0;
    int right = IsKeyDown( KEY_RIGHT ) ? 1 : 0;
    player->vel.x = left * player->walkingSpeed + right * player->walkingSpeed;

    if ( IsKeyPressed( KEY_SPACE ) && player->jumpCount < player->maxJumps ) {
        player->vel.y = player->jumpSpeed;
        player->jumpCount++;
    }

    if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
        for ( int i = 0; i < map->lines; i++ ) {
            for ( int j = 0; j < map->columns; j++ ) {
                int p = i * map->columns + j;
                Block *b = &map->blocks[p];
                if ( !b->broken ) {
                    Vector2 mousePos = GetScreenToWorld2D( GetMousePosition(), *camera );
                    if ( CheckCollisionPointRec( mousePos, b->rect ) ) {
                        b->broken = true;
                    }
                }
            }
        }
    }

}