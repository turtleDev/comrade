/*
 *
 *      notification_sound.c - notificaion sound
 *
 *      TODO: add description and copyright notice.
 *
 */

#include <string.h>

#include "config.h"
#include "debug.h"
#include "path.h"

#ifdef __linux
#include "include/gorilla/ga.h"
#include "include/gorilla/gau.h"
#endif

/* TODO: include gorrila headers on win machines */

static const char *formats[] = {"wav"};
// you'll have to update this manually. No way around it for now, I'm afraid.
static const int f_count = 1;

// is_format_supported(file, fmt) takes two args, a filename and a char buffer.
// it checks if the said format exists in the global formats[] variable 
// and if it does, it copies the format to fmt. returns 1 for True and 0 for
// False.

static int is_format_supported(const char *file, char *fmt) {
    int l = 0;
    for(l = strlen(file) -1; file[l] != '.'; --l) {
        if( l == 0 ) {
            // no extension found
            return 0;
        }
    }
    // since l currently indexes to '.(extension)', we want to move it one
    // index forward so it indexes to (extension) instead
    ++l;

    char ext[256];
    strcpy(ext, file + l);

    int i;
    for(i = 0; i < f_count; ++i) {
        if(!strcmp(formats[i], ext)) {
            strcpy(fmt, ext);
            return 1;
        }
    }
    
    return 0;
}

int play_notification_sound(const char *file) {

    if(!file) return -1;

    if( !path_isfile(file) ) {
        return -1;
    }

    // since filename size have a limit of 256 chars, I've made this buffer
    // equal 256 chars, because I'm paranoid as ... yeah.
    char fmt[256] = {"wav"};

    if(!is_format_supported(file, fmt)) {
        log_err("Unsupported audio format");
        return -1;
    }
    
    gc_initialize(0);
    gau_Manager *mgr = gau_manager_create(0);
    ga_Mixer *mixer = gau_manager_mixer(mgr);
    ga_Sound *snd = gau_load_sound_file(file, fmt);
    ga_Handle *handle = gau_create_handle_sound(mixer, snd, 
                                                &gau_on_finish_destroy, 0, 0);

    ga_handle_play(handle);
    while(handle->state !=  GA_HANDLE_STATE_DESTROYED) {
        gau_manager_update(mgr);
        gc_thread_sleep(1);
    }

    gau_manager_destroy(mgr);
    gc_shutdown();
    return 0;
}
