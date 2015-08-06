/*
 *
 *      config.h - Configuration module 
 *
 *      TODO: add description and copyright notice
 *
 */

#ifndef __CONFIG_H
#define __CONFIG_H

struct Config {
    char *message;
    char *title;
    char *sound_file;
    char *icon_file;
    char *address;
    int ping_count;
};


struct Config * config_load(const char *json);

void config_cleanup(struct Config *cfg);

#endif
