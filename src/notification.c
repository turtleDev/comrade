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
 *
 *      notification.c - display pop-up notification
 *
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wordexp.h>

#ifdef __linux
#include <libnotify/notify.h>
#include <glib.h>
#endif 

#include "notification.h"
#include "path.h"
#include "notification_sound.h"
#include "debug.h"


#ifdef __linux

/**
 * get full path returns the path after 
 * filename/shell expansions
 */
static char *get_full_path(char *file) {

    if ( !file ) {
        return NULL;
    }

    char *path = NULL;
    wordexp_t result;
    int rv = wordexp(file, &result, 0);
    if ( !rv ) {
        path = strdup(result.we_wordv[0]);
        wordfree(&result);
    } else {
        path = strdup(file);
    }

    return path;
   
}


int _display_notification_linux(struct Config *cfg) {
    int icon_exists = 0;
    int sound_exists = 0;

    if(cfg->icon_file && path_isfile(cfg->icon_file)) {
        icon_exists = 1;
    }

    if(cfg->sound_file && path_isfile(cfg->sound_file)) {
        sound_exists = 1;
    }

    // libnotify's icon only shows up if you tell it the absolute path to
    // to icon. It won't show other wise. It returns a freshly allocated
    // string. remember to free it afterwards.
    char *icon = icon_exists ? get_full_path(cfg->icon_file) : NULL;
    char *snd = sound_exists ? get_full_path(cfg->sound_file) : NULL;
    char *title = cfg->title;
    char *msg = cfg->message;

    NotifyNotification *notification = NULL;

    if(notify_init(" ") != TRUE) {
        log_err("unable to initialize libnotify");
        return -1;
    }
    
    notification = notify_notification_new(title, msg, icon);

    // set urgency
    if ( cfg->urgency < 0 || cfg->urgency > 2 ) {
        cfg->urgency = 0;
    }
    notify_notification_set_urgency(notification, cfg->urgency);

    // set timeout
    notify_notification_set_timeout(notification, NOTIFY_EXPIRES_DEFAULT);
    
    if(!notify_notification_show(notification, NULL)) {
        log_err("unable to send notification");
        return -1;
    }

    // play the sound
    if(snd) {
        notification_sound_play(snd);
    }
  
    // free the notifiction object
    g_object_unref(G_OBJECT(notification));
   
    // free strings returned from get_full_path
    if(icon) free(icon);
    if(snd) free(snd);

    // everything went ok. Return success
    return 0;
}

#endif
