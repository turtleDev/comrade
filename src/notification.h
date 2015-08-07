/*
 *
 *      notification.h - pop-up notification module
 *
 *      TODO: Add description and copyright notice.
 *
 */

#ifndef __NOTIFICATION_H
#define __NOTIFICATION_H

#include "config.h"

int _display_notification_linux(struct Config *cfg);

#ifdef __linux__
#define display_notification(cfg) _display_notification_linux(cfg)
#endif

#endif
