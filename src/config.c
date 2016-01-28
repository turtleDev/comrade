
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
 *      config.c - configuration module
 *
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>

#if defined(__linux__)

#include <sys/types.h>
#include <sys/stat.h>

#endif

#include "debug.h"
#include "config.h"
#include "path.h"



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


#ifdef _WIN32
/**
 * on windows, strndup will _probably_ not be available.
 * so we'll provide our own implementation for it
 */

static char *strndup(const char *src, size_t len) {
    /**
     * make sure the source string is long enough
     * for the given input length. If not, then use
     * the length of the input string
     */
    int slen = strlen(src);
    len = len > slen?slen:len;

    // make space for the NULL byte
    len += 1; 

    char *buffer = calloc(len +1, sizeof(char));
    int i;

    // copy the characters
    for ( i = 0; i < (len -1); ++i ) {
        buffer[i] = src[i];
    }

    return buffer;
}
#endif

/**
 * str_indexof(string,ch) returns the index where
 * the first occurance of ch is found in string
 *
 * @param string string to search in
 * @param ch character to find
 *
 * @returns positive int, if ch was found in string,
 * else -1
 */
static int str_indexof(const char *string, char ch) {
    int i;
    for ( i = 0; i < strlen(string); ++i ) {
        if ( string[i] == ch ) {
            return i;
        }
    }
    
    return -1;
}


/**
 *
 * strip(str): removes leading and trailing white space
 * The argument must be a dynamically allocated string.
 *
 * @param str the string to be stripped. this will be free()'d
 * @return a pointer to a newly allocated & stripped string.
 */
static char *strip(char *string) {
    int start, end;
    int i;
 
    // find the start
    for ( i = 0; i < strlen(string); ++i ) {
        if ( isblank(string[i]) ) {
            continue;
        } else {
            start = i;
            break;
        }
    }

    // find the end
    for ( i = strlen(string)-1; i > 0; --i ) {
        if ( isblank(string[i]) ) {
            continue;
        } else {
            end = i;
            break;
        }
    }

    /**
     * find the size of the strip'd string
     *
     * we add since `start` and `end` are indices, which
     * begin from 0, and not 1. 
     */
    int size = end - start +1;

    // strip it
    char *stripped = strndup(string + start, size);

    free(string);

    return stripped;
}


/**
 *
 * begins_with_token(): will find whether any
 * `tokens` are the first non-whitespace
 * characters in `target`
 *
 * @param target string to look in
 * @param tokens char(s) to look for
 *
 * @returns 1 for true, 0 for false
 */

static int begins_with_token(const char *target, char *tokens) {
    int i, j;
    for ( i = 0; i < strlen(target); ++i ) {
        for ( j = 0; j < strlen(tokens); ++j ) {
            if ( target[i] == tokens[j] ) {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * is_comment(string): checks whether a string
 * is a comment
 *
 * @param string string to check
 */
static int is_comment(const char *string) {
   return begins_with_token(string, "#;");
}

/**
 * is_section(string): checks whether a string
 * is a section block
 *
 * @param string string to check
 */
static int is_section(const char *string) {
    return begins_with_token(string, "[");   
}

  
/**
 * another helper. verfies if a string can safely be
 * converted into an int.
 */
static int is_a_number(char *str) {
    uint32_t i;
    uint32_t len = strlen(str);
    for(i = 0; i < len; ++i) {
        unsigned ch = str[i];
        if(!isdigit(ch)) {
            if (ch == '.' || ch == '-') {
                continue;
            }
            return 0;
        }
    }

    return 1;
}


/**
 * struct load_opts used by load_data()
 */
struct load_opts {
    char *token_name;
    char *type;
    void *target;
};

/**
 * load_data(cfg, data, opts, opts_len) will load data into
 * cfg depending on opts passed to it
 *
 * @param cfg Config object to load data into
 * @param data the data to be loaded
 * @param opts list of load_opts objects
 * @param opts_len number of opts object passed
 *
 * @returns 0 if data was sucessfully loaded into cfg,
 * else -1
 */

/**
 * Dark witchcraft. Beware.
 */
static int load_data(char *key, char *value, struct load_opts opts[], int opts_len)  {
    int i;
    for ( i = 0; i < opts_len; ++i ) {

        if (!strcmp(key, opts[i].token_name)) {
            char t = opts[i].type[0];

            if( t == 's' ) {
                /* string */
                char **str_dest = (char **) opts[i].target;
                if ( *str_dest != NULL ) {
                    free(*str_dest);
                    *str_dest = NULL;
                }
                *str_dest = strdup(value);
                return 0;
            } else if ( t == 'i' ) {
                /* integer */
                int *int_dest = (int *) opts[i].target;
                if(is_a_number(value)) {
                    *int_dest = abs(atoi(value));
                }
                return 0;
            } else if ( t == 'f' ) {
                /* float */
                float * float_dest = (float *) opts[i].target;
                if(is_a_number(value)) {
                    *float_dest = atof(value);
                }
                return 0;
            } else if ( t == 'c' ) {
                /* char */
                char *ch_dest = (char *) opts[i].target;
                *ch_dest = value[0];
                return 0;
            } 
        }
    }
    return -1;
}

struct Config *config_load(const char *cfg_string) {

    /**
     * copy the cfg_string into a new buffer, since 
     * we're going to use strtok to parse it.
     *
     * The reason behind this is to prevent any
     * segmentation faults, when static strings 
     * ( or in other words, non-dynamically allocated strings)
     * are passed in as argument.
     */
    char *raw = strdup(cfg_string);

    struct Config *cfg = NULL;
    cfg = malloc(sizeof(struct Config));
    check(cfg != NULL, "out of memory");

    memset(cfg, '\0', sizeof(struct Config));

    char *tok = strtok(raw, "\n");

    struct load_opts opts[] = {
        {"message", "s", &(cfg->message)},
        {"title", "s", &(cfg->title)},
        {"sound_file", "s", &(cfg->sound_file)},
        {"icon_file", "s", &(cfg->icon_file)},
        {"address", "s", &(cfg->address)},
        {"timeout", "f", &(cfg->timeout)},
        {"urgency", "i", &(cfg->urgency)},
        {"ping_count", "i", &(cfg->ping_count)},
    };

    // manually update this
    int opts_len = 8;

    // begin processing tokens
    while (tok != NULL) {

        // find the 'equals' sign in a token 
        int eq = str_indexof(tok, '=');
        if ( eq != -1 || ( !is_comment(tok) && !is_section(tok) ) ) {

            char *key = NULL;
            char *value = NULL;

            // extract key & value from the string
            key = strndup(tok, eq);
            value = strdup(tok + (eq +1)); 

            // and strip and whitespaces
            key = strip(key);
            value = strip(value);

            load_data(key, value, opts, opts_len);

            free(key);
            free(value);
        }

        tok = strtok(NULL, "\n");
    } 

    /**
     * set defaults
     */

    if ( cfg->timeout == 0 ) {
        cfg->timeout = 60;
    }

    if ( cfg->ping_count == 0 ) {
        cfg->ping_count = 4;
    }

    free(raw);
    return cfg;
error:
    exit(-1);
}

void config_cleanup(struct Config *cfg) {
    if(cfg) {
        if(cfg->title) free(cfg->title);
        if(cfg->message) free(cfg->message);
        if(cfg->sound_file) free(cfg->sound_file);
        if(cfg->icon_file) free(cfg->icon_file);
        if(cfg->address) free(cfg->address);
        free(cfg);
    }
}


