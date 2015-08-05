/*
 *
 *
 *      notification.c - display pop-up notification
 *
 *      TODO: add description and copyright notice.
 *
 */

#include <libnotify/notify.h>
#include <glib.h>
#include <unistd.h>

#include "notification.h"
#include "path.h"
#include "notification_sound.h"
#include "debug.h"

int _display_notification_linux(struct Config *cfg) {
    int icon_exists = 1;
    int sound_exists = 1;

    if(!path_isfile(cfg->icon_file)) {
        icon_exists = 0;
    }

    if(!path_isfile(cfg->sound_file)) {
        sound_exists = 0;
    }


    char *msg = cfg->message;
    char *title = cfg->title;
    char *icon = icon_exists ? cfg->icon_file : NULL;
    char *snd = sound_exists ? cfg->sound_file : NULL;

    NotifyNotification *notification = NULL;

    if(notify_init(" ") != TRUE) {
        log_err("unable to initialize libnotify");
        return -1;
    }
    
    notification = notify_notification_new(title, msg, icon);

    notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);

    // set timeout for 5 seconds
    notify_notification_set_timeout(notification, 5000);
    
    if(!notify_notification_show(notification, NULL)) {
        log_err("unable to send notification");
        return -1;
    }

    // play the sound
    play_notification_sound(snd);
  
    // free the notifiction object
    g_object_unref(G_OBJECT(notification));

    // everything went ok. Return success
    return 0;
}
