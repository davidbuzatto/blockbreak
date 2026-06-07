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

    rm.terrainsTexture = LoadTexture( "resources/images/terrain/sheet.png" );
    rm.cracksTexture = LoadTexture( "resources/images/terrain/cracks.png" );

    SetTextureFilter( rm.terrainsTexture, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.cracksTexture, TEXTURE_FILTER_POINT );

    //rm.soundExample = LoadSound( "resources/sfx/powerUp.wav" );
    //rm.musicExample = LoadMusicStream( "resources/musics/overworld1.ogg" );

}

void unloadResourcesResourceManager( void ) {

    UnloadTexture( rm.terrainsTexture );
    UnloadTexture( rm.cracksTexture );

    //UnloadSound( rm.soundExample );
    //UnloadMusicStream( rm.musicExample );
    
}