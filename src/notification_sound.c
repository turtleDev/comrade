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

#ifdef __linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#endif


/* TODO: include gorrila headers on win machines */

/**
 * TODO: add code to differentiate between 'sample' sound likes
 * like WAV, OGG etc and between streaming(?) sounds like mp3,
 * and then handle them appropriately
 */

int play_notification_sound(const char *file) {

    /** 
     * check if the file actually exists
     */
    if ( !file || !path_isfile(file) ) {
        return -1;
    }

    // initialize the SDL audio module
    check(SDL_Init(SDL_INIT_AUDIO) >= 0, "Error: %s", SDL_GetError());
   
    // initialize SDL_mixer module
    check(
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == 0,
        "Error: %s", SDL_GetError()
    );

    // load the music file
    Mix_Chunk *sound = NULL;
    check((sound = Mix_LoadWAV(file)) != NULL, "Error: %s", SDL_GetError());

    /**
     * play music, but don't loop
     */
    Mix_PlayChannel(-1, sound, 0);

    /**
     * NOTE: SDL will automatically play sounds in a seperate thread,
     * that will not DIE if the program gets a SIGINT. In other words,
     * we are running in a seprate thread here, so we'll have to check
     * for ourselves if we need to stop doing, what we're doing
     */

    /**
     * SDL_Event is a union that is used to store an event
     * being processed
     */
    SDL_Event e;

    bool playing = true;

    while ( playing ) {
        while ( SDL_PollEvent(&e) ) {
            // check if the main thread wants to die.
            if ( e.type == SDL_QUIT ) {
                playing = false;
            }
        }
       
        // check if music has stopped playing
        if ( !Mix_Playing(-1) ) {
            playing = false;
        }

        /**
         * by using SDL_Delay(), we make sure we don't
         * cause a burn lock. This is an optimization;
         * Otherwise, the CPU will loop at full speed, 
         * buring power for no reason
         */
        SDL_Delay(500);
    }

    SDL_Quit();

    return 0;

error:
    SDL_Quit();

    return -1;
}
