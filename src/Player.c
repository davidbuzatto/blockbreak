#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"

#include "Block.h"
#include "BlockRange.h"
#include "Macros.h"
#include "Map.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Utils.h"

static void input( Player *player, Map *map, Camera2D *camera );
static void update( Player *player, Map *map, float delta );
static void draw( Player *player );

static void resolveCollisionMapX( Player *player, Map *map );
static void resolveCollisionMapY( Player *player, Map *map );
static void resolveBlockDestruction( Player *player, Map *map, Camera2D *camera );

static bool mouseRightDown = false;

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

    new->lookingRight = true;

    new->walkFrameTime = 0.1f;
    new->walkFrameCounter = 0.0f;
    new->currentWalkFrame = 0;
    new->totalWalkFrames = 8;

    new->pickaxePos = (Vector2) { 30, 25 };
    new->pickaxeStartAngle = 30.0f;
    new->pickaxeAngle = 0.0f;
    new->pickaxeAngleVel = 720.0f;
    new->swingPickaxe = false;

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

static void input( Player *player, Map *map, Camera2D *camera ) {

    int left = IsKeyDown( KEY_LEFT ) ? -1 : 0;
    int right = IsKeyDown( KEY_RIGHT ) ? 1 : 0;
    player->vel.x = left * player->walkingSpeed + right * player->walkingSpeed;

    if ( IsKeyDown( KEY_LEFT ) ) {
        player->lookingRight = false;
    } else if ( IsKeyDown( KEY_RIGHT ) ) {
        player->lookingRight = true;
    }

    if ( IsKeyPressed( KEY_SPACE ) && player->jumpCount < player->maxJumps ) {
        player->vel.y = player->jumpSpeed;
        player->jumpCount++;
    }

    mouseRightDown = IsMouseButtonDown( MOUSE_BUTTON_RIGHT );

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) || mouseRightDown ) {
        resolveBlockDestruction( player, map, camera );
    }

}

static void update( Player *player, Map *map, float delta ) {

    if ( player->vel.x != 0.0f ) {
        player->walkFrameCounter += delta;
        if ( player->walkFrameCounter >= player->walkFrameTime ) {
            player->walkFrameCounter = 0.0f;
            player->currentWalkFrame++;
        }
    } else {
        player->walkFrameCounter = 0.0f;
        player->currentWalkFrame = 0;
    }

    if ( player->swingPickaxe ) {
        player->pickaxeAngle += player->pickaxeAngleVel * delta;
        if ( player->pickaxeAngle >= 360.0f ) {
            player->pickaxeAngle = 0.0f;
            if ( !mouseRightDown ) {
                player->swingPickaxe = false;
            }
        }
    }

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

    //DrawRectangleRec( player->rect, player->color );

    DrawTexturePro(
        rm.playerTexture,
        (Rectangle) { 
            32 * ( player->currentWalkFrame % player->totalWalkFrames ),
            0, 
            player->lookingRight ? 30 : -30,
            50
        },
        player->rect,
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    DrawTexturePro(
        rm.pickaxeTexture,
        (Rectangle) { 0, 0, 25, 43 },
        (Rectangle) { 
            player->lookingRight ? 
                (int) ( player->rect.x + player->pickaxePos.x ) :
                (int) ( player->rect.x + player->rect.width - player->pickaxePos.x ), 
            player->rect.y + player->pickaxePos.y, 
            25, 43
        },
        (Vector2) { 12, 40 },
        player->lookingRight ? player->pickaxeAngle + player->pickaxeStartAngle : - ( player->pickaxeAngle + player->pickaxeStartAngle ),
        WHITE
    );


}

static void resolveCollisionMapX( Player *player, Map *map ) {

    BlockRange range = getNeighborBlocks( map, player->rect );

    for ( int i = range.rowMin; i <= range.rowMax; i++ ) {
        for ( int j = range.colMin; j <= range.colMax; j++ ) {
            int p = i * map->columns + j;
            Block *b = &map->blocks[p];
            if ( !b->broken ) {
                if ( CheckCollisionRecs( player->rect, b->rect ) ) {
                    if ( player->vel.x > 0.0f ) {
                        player->rect.x = b->rect.x - player->rect.width;
                    } else if ( player->vel.x < 0.0f ) {
                        player->rect.x = b->rect.x + b->rect.width;
                    } else {
                        // tie resolution for vel.x == 0
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

    BlockRange range = getNeighborBlocks( map, player->rect );

    for ( int i = range.rowMin; i <= range.rowMax; i++ ) {
        for ( int j = range.colMin; j <= range.colMax; j++ ) {
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

static void resolveBlockDestruction( Player *player, Map *map, Camera2D *camera ) {

    BlockRange range = getNeighborBlocks( map, player->rect );

    // start pickage swing when pressing with left or right buttons
    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) || IsMouseButtonPressed( MOUSE_BUTTON_RIGHT ) ) {
        player->pickaxeAngle = 0.0f;
        player->swingPickaxe = true;
    }

    for ( int i = range.rowMin; i <= range.rowMax; i++ ) {
        for ( int j = range.colMin; j <= range.colMax; j++ ) {
            int p = i * map->columns + j;
            Block *b = &map->blocks[p];
            if ( !b->broken ) {
                Vector2 mousePos = GetScreenToWorld2D( GetMousePosition(), *camera );
                if ( CheckCollisionPointRec( mousePos, b->rect ) ) {
                    b->hits++;
                    if ( b->hits == b->hitsToBreak ) {
                        b->broken = true;
                    }
                }
            }
        }
    }

}
