/*
 *      Copyright (C) 2015 Saravjeet 'Aman' Singh
 */

/*   This file is a part of Comrade.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */


/*
 *
 *      notification_sound.c - notification sound
 *
 */

#include <string.h>
#include <stdbool.h>

#include "config.h"
#include "debug.h"
#include "path.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


/**
 * has_extension(filename, ext) returns true if filename has ext
 * extention, else it returns false.
 *
 * @param filename the name of the file
 * @param ext the extention to check
 * 
 * @return true if the file name has ext extention, else false.
 */

bool has_extension(const char *filename, char *ext) {
    int index;
    
    // There is a library function for this called strrchr()
    for ( index = strlen(filename) -1; index > 0; --index ) {
        if ( filename[index] == '.' ) {
            /**
             * index currently points to the '.', we want
             * to increment it to the start of the substring
             * that forms the extension of the filename
             */
            ++index;
            break;
        }
    }

    const char *file_ext = filename + index;
    
    /**
     * if index > 0 ( which basically means the filename 
     * has an extension ) and if the ext argument is 
     * the same as the filename extension, then return
     * true, else false.
     */
    // It is idiomatic in C to directly use boolean expressions
    // true and false are not standard C 
    // Nor are this sort of comment for that matter!
    // In any case, just 
    // return index && !strcmp(file_ext, ext);
    // Is a more clear way of expressing the code
    // Unlike python, implicit is preferred in C over explicit  
     
    if ( index != 0 && !strcmp(file_ext, ext)) {
        return true;
    } else {
        return false;
    }
    
    // Ultimately instead of this complicated approach, why not just compare strlen(ext) bytes
    // from the end of filename against ext and also that the byte before that is a'.'
    // Just one strlen, one strcmp and some checks to ensure filename is longer than ext
}
   
/**
 * is_music is used to determine if a file is a 
 * 'streaming' file like an mp3, or a chunk file,
 * like WAV, OGG etc.
 *
 * @param filename the name of the file
 *
 * @return true if the filename is a streaming file,
 * false if it is a non-streaming file.
 */
bool is_music(const char *filename)
{
    char *streams[] = {"mp3"};
    int len = sizeof(streams)/sizeof(streams[0]);
    int i;

    for ( i = 0; i < len; ++i ) {
        if ( has_extension(filename, streams[0]) ) {
            return true;
        }
    }

    return false;
}

/**
 *
 * notification_sound_init()
 *
 * initialize SDL and it's subsystems
 *
 * @returns 1 for success, 0 for error
 */
int notification_sound_init(void) {

    check(SDL_Init(SDL_INIT_EVERYTHING) >= 0, "Error: %s", SDL_GetError());
    check(
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == 0, 
        "Error: %s", SDL_GetError()
    );

    return 1;

error:
    return 0;
}

/**
 *
 * notification_sound_cleanup()
 *
 * shutdown SDL and it's subsystems
 *
 * @returns nothing
 */
void notification_sound_cleanup(void) {
    Mix_CloseAudio();
    SDL_Quit();
}

int notification_sound_play(const char *file) {

    /** 
     * check if the file actually exists
     */
    if ( !file || !path_isfile(file) ) {
        return -1;
    }

    /*
    // initialize the SDL audio module
    check(SDL_Init(SDL_INIT_EVERYTHING) >= 0, "Error: %s", SDL_GetError());
   
    // initialize SDL_mixer module
    check(
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == 0,
        "Error: %s", SDL_GetError()
    );
    */


    // load the music file
    Mix_Chunk *chunk = NULL;
    Mix_Music *music = NULL;

    if ( is_music(file) ) {
        check((music = Mix_LoadMUS(file)) != NULL, "Error: %s\n", SDL_GetError());

        // play the music, but don't loop
        Mix_VolumeMusic(MIX_MAX_VOLUME);
        Mix_PlayMusic(music, 0);
    } else {
        check((chunk = Mix_LoadWAV(file)) != NULL, "Error: %s\n", SDL_GetError());
    
        // play the sound, but don't loop
        Mix_PlayChannel(-1, chunk, 0);
    }

    /**
     * SDL_Event is a union that is used to store an event
     * being processed
     */
    SDL_Event e;

    bool playing = true;

    while ( playing ) {
        while ( SDL_PollEvent(&e) ) {
            if ( e.type == SDL_QUIT ) {
                playing = false;
            }
        }
       
        // check if music has stopped playing
        if ( !Mix_Playing(-1) && !Mix_PlayingMusic() ) {
            playing = false;
        }

        /**
         * by using SDL_Delay(), we make sure we don't
         * cause a burn loop. This is an optimization;
         * Otherwise, the CPU will loop at full speed, 
         * buring power for no reason.
         */
        SDL_Delay(1.0/30.0);
    }

    if (music) {
        Mix_FreeMusic(music);
    }

    if (chunk) {
        Mix_FreeChunk(chunk);
    }
    /*
    SDL_Quit();
    Mix_CloseAudio();
    */
    return 0;

error:
    if (chunk) Mix_FreeChunk(chunk);
    /*
    SDL_Quit();
    Mix_CloseAudio();
    */

    return -1;
}
