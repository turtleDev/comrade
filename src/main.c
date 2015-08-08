/*
 *
 *
 *      main.c - main program module
 *
 *      TODO: add description and copyright notice
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "debug.h"
#include "config.h"
#include "notification.h"
#include "ping.h"
#include "lock.h"

char *read_file(FILE *f) {
    int count = 0;
    while(fgetc(f) != EOF) ++count;
    rewind(f);
    
    char *str = malloc(sizeof(char) * (count+1));
    char ch;
    int i;

    for(i = 0; i < count; ++i) {
        ch = fgetc(f);
        if(ch == EOF) {
            str[i] = '\0';
            break;
        }
        str[i] = ch;
    }
    return str;
}

const char *DEFAULT_CONFIG = \
"{ \"title\": \"notification\", \
   \"message\": \"your internet is now working\", \
   \"address\": 127.0.0.1, \
   \"ping_count\": 4 , \
   \"timeout\": 5 }";    

const char *MSG_ANOTHER_INSTANCE = \
"{ \"title\": \"Comrade\", \
   \"message\":\"Another instance is already running\" }";

const char *MSG_TIMEOUT = \
"{ \"title\":\"Comrade\", \
   \"message\":\"Timed out after %.2f minutes and %d failed attempts\"}";

void usage(void) {
    char *usage_string = \
        "usage: comrade [-h|--help]";
    printf("%s\n", usage_string);
    return;
}

int main(int argc, char *argv[]) {

    int i;
    for(i = 1 ; i < argc; ++i) {
        if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            usage();
            return 0;
        }
    }

    if(lock_acquire(argv[0])) {

        // if we're unable to acquire lock
        struct Config *cfg;
        cfg = config_load(MSG_ANOTHER_INSTANCE);
        
        // if we're unable to parse config string for some reason or
        // in case display_notification fails, log the message.
        if(!cfg || display_notification(cfg)) {
            log_err("Another instance is already running");
        }
        config_cleanup(cfg);
        return -1;
    }

    char *config_file = "config.json";
    FILE *f;
    struct Config *cfg = NULL;

    f = fopen(config_file, "r+");
    
    // if we're unable to open the configuration file.
    if(!f) {
        log_err("unable to open the file: %s", config_file);
        errno = 0;
    } else {
        char *config_string = read_file(f);
        fclose(f);
        cfg = config_load(config_string);
        
        // unable to parse configuration string.
        if(!cfg) {
            log_warn("unable to parse configuration file: %s", config_file);
            log_info("using default configuration");
        }
    }

    if(!cfg) cfg = config_load(DEFAULT_CONFIG);

    int rc;
    time_t start_time = time(NULL);
    int attempts = 1;
    double diff;
    while(1) {
        // if the time since start exceeds the timeout duration(in minutes),
        // then log and exit.
        diff = difftime(time(NULL), start_time);
        if( diff > (cfg->timeout *60)) {
           

            // allocate space for our msg. +16 is padding for
            // any printf style text substitution. This is just a safety 
            // measure.
            char *msg = malloc(sizeof(char) * (strlen(MSG_TIMEOUT) + 16));

            // convert diff to minutes
            diff = diff/60;
            sprintf(msg, MSG_TIMEOUT, diff, attempts);
            
            struct Config *timeout_cfg = config_load(msg);
            
            if(!timeout_cfg || display_notification(timeout_cfg)) {
                log_err("timed out after %.2f minutes and %d failed attempts",
                        diff, attempts);
            }
            free(msg);
            config_cleanup(timeout_cfg);
            break;
        }
        
        rc = ping(cfg->address, cfg->ping_count);

        if( rc == 0 ) {
            display_notification(cfg);
            break;
        } else {
            sleep(4);
        }
        ++attempts;
    }
    lock_release();
    config_cleanup(cfg);
    return 0;
}
