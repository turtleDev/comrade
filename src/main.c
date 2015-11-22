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
#include <math.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "debug.h"
#include "config.h"
#include "notification.h"
#include "ping.h"
#include "lock.h"
#include "path.h"


const char *DEFAULT_CONFIG = \
"{ \"title\": \"Comrade\", \
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

const char *USAGE = "\
Comrade: Know when your internet is up              \n\
                                                    \n\
usage: comrade [OPTIONS]                            \n\
                                                    \n\
options:                                            \n\
        -h, --help :     display this message       \n\
        -c, --count n:   ping for atleast n count   \n\
        -w, --website x: website to ping            \n\
        -t, --timeout m: timeout after m minutes    \n\
                                                    \n\
Note: Comrade will try to load a file called        \n\
      'config.json', from the directory where the   \n\
      executable is present to configure itself.    \n\
      If it is unable to load that file, then it    \n\
      will use a default configuration that is      \n\
      built into it. If any command line option     \n\
      is passed to Comrade, then such option will   \n\
      overide that particular part of Comrade's     \n\
      configuration.                                \n\
                                                    \n\
    Copyright (C) 2015 Saravjeet Aman Singh         \n\
    <saravjeetamansingh@gmail.com>                  \n\
";


/**
 * read_file() reads a file and stores its content in an array.
 * The array is allocated from heap, so remember to free it.
 * returns NULL on error.
 */

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


/**
 * replace dest string with src string.
 *
 * Assumes that both were dynamically allocated
 */
void replace_str(char *src, char *dest) {
    if(dest) free(dest);
    dest = strdup(src);
}


/**
 * get_config_path() returns the path to the 
 * configuration file config.json.
 */
char *get_config_path(void) {
    char *home = getenv("HOME");
    char *path = NULL;

    if (home) {
        // create a path string to $HOME/.local
        int len = strlen(home) + strlen("/.local") + 1;
        path = malloc(len * sizeof(char));
        check(path != NULL, "out of memory");

        sprintf(path, "%s/%s", home, ".local");

        // check if $HOME/.local exists
        if (path_isdir(path)) {
            // create path to $HOME/.local/Comrade
            len = strlen(path) + strlen("/Comrade") + 1;
            check(
                (path = realloc(path, sizeof(char) * len)) != NULL,
                "out of memory"
            );
    
            strcat(path, "/Comrade");

            // check for its existance
            if ( !path_isdir(path) ) {
                check(
                    mkdir(path, 0777) == 0, "unable to create directory"
                );
            }

            // now create the final path to config.json
            len = strlen(path) + strlen("/config.json") + 1;
            check(
                (path = realloc(path, sizeof(char) * len)) != NULL,
                "out of memory"
            );

            strcat(path, "/config.json");

            return path;

        } else {
            
            // $HOME/.local does not exist. so we're going 
            // to use $HOME/.comrade.json instead
            free(path);

            len = strlen(home) + strlen("/.comrade.json") + 1;
            path = malloc(sizeof(char) * len);
            check(path != NULL, "out of memory");
            
            sprintf(path, "%s/%s", home, ".comrade.json");

            return path;
        }

    } else {
        // Since we got no home, we can't afford no configuration
        return NULL;
    }

error:
    if (path) free(path);
    return NULL;
}

int main(int argc, char *argv[]) {

    int i;
    for(i = 1 ; i < argc; ++i) {
        if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            printf("%s", USAGE);
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
              fprintf(stderr,"Another instance is already running\n");
        }
        config_cleanup(cfg);
        return -1;
    }

    char *config_file = get_config_path();
    FILE *f;
    struct Config *cfg = NULL;

    f = fopen(config_file, "r+");
    
    // if we're unable to open the configuration file.
    if(!f) {
        log_err("unable to open the file: %s", config_file);
        errno = 0;
    } else {
        char *config_string = read_file(f);
        cfg = config_load(config_string);

        // free up the resources that we no longer need
        fclose(f);
        
        // unable to parse configuration string.
        if(!cfg) {
            log_warn("unable to parse configuration file: %s", config_file);
            log_info("using default configuration");
        }

        free(config_string);
    }

    if(!cfg) {
        // load default config
        cfg = config_load(DEFAULT_CONFIG);
        
        // write the default configuration to disk.
        if (config_file) {
            FILE *cf = NULL;
            cf = fopen(config_file, "w");

            if (cf) {
                fprintf(cf, "%s", DEFAULT_CONFIG);
                fclose(cf);
            }
        }
    }
  
    // free out the strings that we no longer need
    free(config_file);
    
    /**
     * TODO: replace the following block of code with a version 
     *       that uses getopt_long instead.
     */
    for(i = 1; i < argc; ++i) {
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
                int num = abs(atoi(argv[i+1]));
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
        if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--timeout")) {
            if(argv[i+1] != NULL) {
                float minutes = fabs(atof(argv[i+1]));
                if(minutes == 0.0) {
                    fprintf(stderr, "Comrade: invalid timeout duration\n");
                    config_cleanup(cfg);
                    return -1;
                }
                cfg->timeout = minutes;
            } else {
                fprintf(stderr, "Comrade: was expecting timeout duration\n");
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
        if((cfg->timeout != -1) && diff > (cfg->timeout *60)) {
           

            // allocate space for our msg. +16 is padding for
            // any printf style text substitution. This is just a safety 
            // measure.
            char *msg = malloc(sizeof(char) * (strlen(MSG_TIMEOUT) + 16));

            // convert diff to minutes
            diff = diff/60;
            sprintf(msg, MSG_TIMEOUT, diff, attempts);
            
            timeout_cfg = config_load(msg);
            
            if(!timeout_cfg || display_notification(timeout_cfg)) {
                fprintf(stderr,
                        "timed out after %.2f minutes and %d failed attempts\n",
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
