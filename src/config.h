/*
 *
 *      config.h - Configuration module 
 *
 *      TODO: add description and copyright notice
 *
 */


struct Config {
    char *message;
    char *title;
    char *sound_file;
    char *icon_file;
};


struct Config * config_load(const char *json);

void config_cleanup(struct Config *cfg);

