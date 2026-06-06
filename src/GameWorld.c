/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "stb/stb_perlin.h"
#define STB_PERLIN_IMPLEMENTATION

#include "Typedefs.h"
#include "GameWorld.h"
#include "Map.h"
#include "Player.h"
#include "ResourceManager.h"

static void updateCamera( GameWorld *gw );

GameWorld *createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    gw->player = createPlayer( 2000, 100, 30, 50, BLUE );
    gw->map = createMap( 0, 150, 200, 200, 20 );

    gw->camera = (Camera2D) {
        .target = { 0 },
        .offset = { 0 },
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    return gw;

}

void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        destroyMap( gw->map );
        destroyPlayer( gw->player );
        free( gw );
    }
}

void updateGameWorld( GameWorld *gw, float delta ) {

    Vector2 mouseWheel = GetMouseWheelMoveV();

    if ( mouseWheel.y > 0.0f ) {
        gw->camera.zoom += 0.1f;
    } else if ( mouseWheel.y < 0.0f ) {
        gw->camera.zoom -= 0.1f;
        if ( gw->camera.zoom <= 0.0f ) {
            gw->camera.zoom = 0.1f;
        }
    }

    if ( IsKeyPressed( KEY_R ) ) {
        gw->camera.zoom = 1.0f;
    }

    gw->player->input( gw->player, gw->map, &gw->camera );
    gw->player->update( gw->player, gw->map, delta );
    updateCamera( gw );

}

void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    BeginMode2D( gw->camera );
    gw->map->draw( gw->map );
    gw->player->draw( gw->player );
    EndMode2D();

    EndDrawing();

}

static void updateCamera( GameWorld *gw ) {

    Player *p = gw->player;
    Camera2D *c = &gw->camera;

    c->offset.x = GetScreenWidth() / 2;
    c->offset.y = GetScreenHeight() / 2;

    c->target.x = roundf( p->rect.x + p->rect.width / 2 );
    c->target.y = roundf( p->rect.y + p->rect.height / 2 );

    // for this game, this does not make sense i think :)
    /*int minX = GetScreenWidth() / 2;
    int maxX = calcMapWidth( gw->map ) - minX;
    int maxY = calcMapHeight( gw->map ) - GetScreenHeight() / 2;
    
    if ( c->target.x < minX ) {
        c->target.x = minX;
    } else if ( c->target.x > maxX ) {
        c->target.x = maxX;
    }

    if ( c->target.y > maxY ) {
        c->target.y = maxY;
    }*/

}