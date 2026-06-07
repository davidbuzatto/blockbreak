/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "ResourceManager.h"
#include "raylib/raylib.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {

    rm.playerTexture = LoadTexture( "resources/images/player/player.png" );
    rm.pickaxeTexture = LoadTexture( "resources/images/player/pickaxe.png" );
    rm.terrainsTexture = LoadTexture( "resources/images/terrain/sheet.png" );
    rm.cracksTexture = LoadTexture( "resources/images/terrain/cracks.png" );
    rm.skyBgTexture = LoadTexture( "resources/images/bg/sky.png" );
    rm.caveBgTexture = LoadTexture( "resources/images/bg/cave.png" );

    SetTextureFilter( rm.playerTexture, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.pickaxeTexture, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.terrainsTexture, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.cracksTexture, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.skyBgTexture, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.caveBgTexture, TEXTURE_FILTER_POINT );

    //rm.soundExample = LoadSound( "resources/sfx/powerUp.wav" );
    //rm.musicExample = LoadMusicStream( "resources/musics/overworld1.ogg" );

}

void unloadResourcesResourceManager( void ) {

    UnloadTexture( rm.playerTexture );
    UnloadTexture( rm.pickaxeTexture );

    UnloadTexture( rm.terrainsTexture );
    UnloadTexture( rm.cracksTexture );

    UnloadTexture( rm.skyBgTexture );
    UnloadTexture( rm.caveBgTexture );

    //UnloadSound( rm.soundExample );
    //UnloadMusicStream( rm.musicExample );

}