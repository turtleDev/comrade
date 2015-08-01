/*
 *
 *      notification_sound.c - notificaion sound
 *
 *      TODO: add description and copyright notice.
 *
 */

#include <string.h>

#include "include/gorilla/ga.h"
#include "include/gorilla/gau.h"
#include "debug.h"

static const char *formats[] = {"wav"};

static int is_format_supported(const char *file, const char *fmt) {
    // TODO: Implement this
    return 1;
}

int play_notification_sound(const char *file) {

    char fmt[16] = {"wav"};

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
    gc_shutdown(mgr);
    return 0;
}
