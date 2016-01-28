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

/* standard library headers */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

/* OS specific headers */
#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#endif /* end __linux__ */

#ifdef _WIN32
#include <windows.h>
#include <Shlwapi.h> /* for SHGetValue */
#endif /* end _WIN32 */

/* project headers */
#include "debug.h"
#include "config.h"
#include "notification_sound.h"
#include "notification.h"
#include "ping.h"
#include "lock.h"
#include "path.h"


const char *DEFAULT_CONFIG = "\
[general] \n\
# lines beginning with # or ; are comments \n\
# modify this file to suit your preference \n\
\n\
# the title of the notification \n\
title=Comrade \n\
\n\
# the message \n\
message=your internet is now working \n\
\n\
# the address to ping \n\
address=www.example.com\n\
\n\
# ping count per run. functionally equal to -c/-n flag to ping command \n\
ping_count=4 \n\
\n\
# timeout for the notification (this value *may be* ignored by comrade) \n\
timeout=5 \n\
\n\
# notification urgency. (may be ignored) \n\
urgency=2\n\
\n\
# path to the sound file to play \n\
# sound_file=<path/to/file> \n\
\n\
# path to the icon to display in the notification \n\
# icon_file=<path/to/icon>\n\
";

/**
 * urgency should be in the range 0-2,
 * where 0 is the lowest urgency, and 2
 * is the highest.
 */
const char *MSG_START = "\
title=Comrade\n\
message=Starting up\n\
urgency=0\
";

const char *MSG_ANOTHER_INSTANCE = "\
title=Comrade \n\
message=Another instance is already running\
";

const char *MSG_TIMEOUT = "\
title=Comrade \n\
message=Timed out after %.2f minutes and %d failed attempts\
";

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
      'comrade.cfg', from {HOME}/.local/Comrade     \n\
      If it is unable to load that file, then it    \n\
      will use the default configuration that is    \n\
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
char *read_file(const char *file)
{

    // find the file size
    struct stat st;
    int rv;
    rv = stat(file, &st);
    if ( rv != 0 ) {
        log_err("stat failed on file %s", file);
        return NULL;
    }
    int size = st.st_size;

    FILE *f = fopen(file, "r");
    if ( f == NULL ) {
        log_err("unable to open file: %s", file);
        return NULL;
    }

    char *buffer = malloc(sizeof(char) * (size+1));

    if ( buffer == NULL ) {
        exit(-1);
        fprintf(stderr, "out of memory\n");
    }

    fread(buffer, sizeof(char), size, f);

    fclose(f);

    buffer[size] = '\0';

    return buffer;
}


/**
 * replace dest string with src string.
 *
 * Assumes that destination was dynamically allocated
 */

void replace_str(char *src, char **dest) {
    if(*dest) free(*dest);
    *dest = strdup(src);
}


/**
 * get_config_path() returns the path to the
 * configuration file comrade.cfg
 */

#if  defined(__linux__)
/**
 * on linux, we will store rc files in  
 * $HOME/.config/Comrade/comrade.cfg 
 */
char *get_config_path(void) {
    
    char *home = getenv("HOME");
    char *path = NULL;

    if (home) {
        // create a path string to $HOME/.config
        int len = strlen(home) + strlen("/.config") + 1;
        path = malloc(len * sizeof(char));
        check(path != NULL, "out of memory");

        sprintf(path, "%s/%s", home, ".config");

        // check if $HOME/.config exists, and create it if it doesn't
        if (path_isdir(path)) {
            mkdir(path, 0777);
        }

        // create path string `$HOME/.config/Comrade`
        len = strlen(path) + strlen("/Comrade") + 1;
        check(
            (path = realloc(path, sizeof(char) * len)) != NULL,
            "out of memory"
        );

        strcat(path, "/Comrade");

        // check for its existance
        if ( !path_isdir(path) ) {
            check(
                mkdir(path, 0777) == 0, "unable to create directory %s", path
            );
        }

        // now create the final path to config file
        len = strlen(path) + strlen("/comrade.cfg") + 1;
        check(
            (path = realloc(path, sizeof(char) * len)) != NULL,
            "out of memory"
        );

        strcat(path, "/comrade.cfg");

        return path;

    } else {
        // Since we got no home, we can't afford no configuration
        return NULL;
    }

error:
    if (path) free(path);
    return NULL;
}

#elif defined(_WIN32)  /* end __linux__ */

/**
 * On windows, we will store comrade's configuration files in 
 * <My Documents>/Comrade/comrade.cfg
 */

char *get_config_path(void) {
    /* TODO: use window's unicode path API to allow for longer path names */
    char buffer[MAX_PATH];
    DWORD length = MAX_PATH -1;
    DWORD type=0;
    
    // return value(s)
    DWORD rv;

    /**
     * <My Documents> is a special folder on windows. On newer system it's
     * generally C:\Users\<USER>\Documents, but it can be configured to
     * be some other folder. We're going to use registry to find out where
     * this folder is. To do this, we're going to have to read the 
     * "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\
     *  Explorer\User Shell Folders" key's value called "Personal".
     */

    /* SHGetValueA, ANSI version of SHGetValue */
    rv = SHGetValueA(HKEY_CURRENT_USER,
                         "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders",
                         "Personal",
                         &type,
                         buffer,
                         &length);

    /* some error occured */
    check( rv == ERROR_SUCCESS, "unable to read registry");


    /**
     * expand any environment variables in the returned path
     *
     * NOTE: according to the documentation at msdn on 
     * ExpandEnvironmentString, the expanded strings are limited to 32k
     * characters. However, I'm only going to keep a buffer PATH_MAX long,
     * since that should suffice 95% of the times. However, It would be 
     * nice if in the future, this function could also handle any possible
     * edge cases.
     */
    char expanded_buffer[PATH_MAX];

    /**
     * ExpandEnvironmentStringsA, the ANSI version
     * of ExpandEnvironmentStrings.
     */
    rv = ExpandEnvironmentStringsA(buffer, expanded_buffer, MAX_PATH -1);

    check( rv != 0, "unable to read environment variables");

    char config_folder[MAX_PATH];
    sprintf(config_folder, "%s\\%s", expanded_buffer, "Comrade");

    if ( !path_isdir(config_folder) ) {
        /* try creating the directory */
        rv = CreateDirectoryA(config_folder, NULL);

        /* create directory failed */
        if ( rv == 0 ) {
            log_err("unable to create %s", config_folder);
            return NULL;
        }
    }

    char *path = (char *)malloc(sizeof(char) * MAX_PATH);

    check(path != NULL, "out of memory");
    
    /* build the final path string to <My Documents>/Comrade/comrade.cfg */
    sprintf(path, "%s\\%s", config_folder, "comrade.cfg");

    return path;

error:
    return NULL;
}

#endif /* end _WIN32 */


/**
 * init()
 *
 * initialize subsystems.
 *
 * @returns 1 for success, 0 for error
 */

int init(void) {
    return notification_sound_init();
}

/**
 * cleanup()
 *
 * shut down subsystems
 *
 * @returns nothing
 */

void cleanup(void) {
    notification_sound_cleanup();
}

/**
 * Entry points for different OSes
 */
#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE hInstance, 
                    HINSTANCE hPrevInstance, 
                    LPSTR lpCmdLine, 
                    int nCmdShow) {
    
    int argc;
    char **argv;
    char **environ;

    __getmainargs(&argc, &argv &environ);

#endif /* end _WIN32 */

#ifdef __linux__

int main(int argc, char *argv[]) {

#endif
  
    /* init anything that needs an init */
    init();

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
    struct Config *cfg = NULL;

    if(!path_isfile(config_file)) {
        log_err("unable to open the file: %s", config_file);
        errno = 0;
    } else {
        char *config_string = read_file(config_file);
        cfg = config_load(config_string);

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
                replace_str(argv[i+1], &(cfg->address));
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

    /**
     * notify the user that comrade has started
     */
    struct Config *starting = config_load(MSG_START);
    display_notification(starting);
    config_cleanup(starting);


    int rc;
    int attempts = 1;
    struct Config *timeout_cfg = NULL;

    time_t start_time = time(NULL);
    time_t diff = 0;

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
            double diff_minutes = diff/60;
            sprintf(msg, MSG_TIMEOUT, diff_minutes, attempts);

            timeout_cfg = config_load(msg);

            if(!timeout_cfg || display_notification(timeout_cfg)) {
                fprintf(stderr,
                        "timed out after %.2f minutes and %d failed attempts\n",
                            diff_minutes, attempts);
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

    /* cleanup */
    cleanup();
    lock_release();
    config_cleanup(cfg);
    return 0;
}
