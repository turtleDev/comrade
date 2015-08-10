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
 *      main.c - main program module
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

// read_file(f) returns a pointer to a string, that points to an array of 
// characters, that represent the contents of the file f. The string is 
// dynamically allocated using malloc. return NULL on error.
char *read_file(FILE *f) {
    int count = 0;
    while(fgetc(f) != EOF) ++count;
    rewind(f);
    
    char *str = malloc(sizeof(char) * (count+1));
    if(!str) {
        return NULL;
    }
    memset(str, '\0', sizeof(char) * (count+1));
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

void replace_str(char *src, char *dest) {
    if(dest) free(dest);

    dest = strdup(src);
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
        check(!cfg || display_notification(cfg),
              "Another instance is already running")
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

    for(i = 0; i < argc; ++i) {
        if(!strcmp(argv[i], "-w") || !strcmp(argv[i], "--website")) {
            if(argv[i+1] != NULL) {
                replace_str(argv[i+1], cfg->address);
            } else {
                fprintf(stderr, "Comrade: was expecting a website\n");
                config_cleanup(cfg);
                return -1;
            }
        }

        if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--count")) {
            if(argv[i+1] != NULL) {
                // ping_count is an int
                int num = atoi(argv[i+1]);
                if(num == 0) {
                    fprintf(stderr, "Comrade: invalid count\n");
                    config_cleanup(cfg);
                    return -1;
                }
                cfg->ping_count = num;
            } else {
                fprintf(stderr, "Comrade: was expecting a ping count\n");
                config_cleanup(cfg);
                return -1;
            }
        }
    }


    int rc;
    time_t start_time = time(NULL);
    int attempts = 1;
    struct Config *timeout_cfg = NULL;
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
            
            timeout_cfg = config_load(msg);
            
            check(!timeout_cfg || display_notification(timeout_cfg),
                  "timed out after %.2f minutes and %d failed attempts",
                        diff, attempts); 
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

error:
    if(cfg) config_cleanup(cfg);
    if(timeout_cfg) config_cleanup(timeout_cfg);
    return -1;
}
