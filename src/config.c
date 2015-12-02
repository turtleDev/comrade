
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

#include "debug.h"

#ifdef __linux__
#include "include/jsmn.h"
#endif

#include "config.h"

/* TODO: include headers using windows path on windows platform */


/**
 * helper function used to compare json data to constant strings
 */
static int isequal(const char *json, jsmntok_t t, const char *s) {
   if(t.type == JSMN_STRING && (strlen(s) == (t.end - t.start)) &&
      !strncmp(json + t.start, s, t.end - t.start)) { 
        return 1;
    }
   return 0;
}

/**
 * getdata() is a helper function that is used to extract data
 * from tokens
 */
static char *getdata(const char *json, jsmntok_t t[], int i) {
    char *str = strndup(json + t[i+1].start,t[i+1].end - t[i+1].start);
    return str;
}
  
/**
 * another helper. very similar to isalpha()
 */
static int is_a_number(char *str) {
    int i;
    int len = strlen(str);
    for(i = 0; i < len; ++i) {
        if(!isdigit(str[i])) {
            if (str[i] == '.' || str[i] == '-'){
                continue;
            }
            return 0;
        }
    }

    return 1;
}


struct Config *config_load(const char *json) {
    jsmn_parser parser;
    jsmn_init(&parser);
    jsmntok_t *tokens = NULL;
    // parse the number of tokens.
    int rc = 0;
    rc = jsmn_parse(&parser, json, strlen(json), NULL, 0);

    if ( rc == JSMN_ERROR_INVAL || rc == JSMN_ERROR_PART ) {
        // json string does not have a complete object or
        // json string has invalid characters, then, return an error
        log_err("configuration JSON string is invalid.");
        return NULL;
    }
    
    tokens = malloc(sizeof(jsmntok_t) * rc);

    if(!tokens) {
        log_err("out of memory");
        return NULL;
    }

    // re-init parser, since we've already used it once to parse the number
    // of tokens.
    jsmn_init(&parser);

    // parse tokens 
    jsmn_parse(&parser, json, strlen(json), tokens, rc);

    // we only expect a single toplevel object. All other objects will be 
    // ignored. 
    if(tokens[0].type != JSMN_OBJECT ) {
        log_err("expected a JSON object");
        return NULL;
    }
 

    struct Config *cfg = malloc(sizeof(struct Config));

    if(!cfg) {
        log_err("out of memory");
        return NULL;
    }

    // set everythin to \0 (null). This makes it safe to 
    // free this structure anytime, without worrying about whether
    // the fields(char *ptrs) were properly initialized or not.
    memset(cfg, '\0', sizeof(struct Config));

  
    // tokens.size is the number of key:value pairs. to index over each token
    // (each key and value) we'll need a limit of twice of size.
    int len = tokens[0].size * 2;
    // I start the index from one, since the index 0 is the toplevel object
    int i;
    for(i = 1; i < len; ++i) {
        if(isequal(json, tokens[i], "title")) {
            // copy the data.
            cfg->title = getdata(json, tokens, i);

            // move the counter, since i+1 has been copied.
            ++i;
            if(!cfg->title) {
                config_cleanup(cfg);
                return NULL;
            }

        } else if(isequal(json, tokens[i], "message")) {
            cfg->message = getdata(json, tokens, i);

            ++i;
            if(!cfg->message) {
                config_cleanup(cfg);
                return NULL;
            }
        } else if(isequal(json, tokens[i], "sound_file")) {
            cfg->sound_file = getdata(json, tokens, i);

            ++i;
            if(!cfg->sound_file) {
                config_cleanup(cfg);
                return NULL;
            }
        } else if(isequal(json, tokens[i], "icon_file")) {
            cfg->icon_file = getdata(json, tokens, i);

            ++i;
            if(!cfg->icon_file) {
                config_cleanup(cfg);
                return NULL;
            }
        } else if(isequal(json, tokens[i], "address")) {
            cfg->address = getdata(json, tokens, i);

            ++i;
            if(!cfg->address) {
                config_cleanup(cfg);
                return NULL;
            }
        } else if(isequal(json, tokens[i], "ping_count")) {
            char *buf = getdata(json, tokens, i);
            ++i;
            
            if(!is_a_number(buf)) {
                log_err("ping_count must be a number");
                free(buf);
                config_cleanup(cfg);
                return NULL;
            }

            cfg->ping_count = abs(atoi(buf));
            free(buf);
        } else if(isequal(json, tokens[i], "timeout")) {
            char *buf = getdata(json, tokens, i);
            ++i;

            if(!is_a_number(buf)) {
                log_err("timeout must be a number");
                free(buf);
                config_cleanup(cfg);
                return NULL;
            }

            cfg->timeout = atof(buf);
            free(buf);
        } else if(isequal(json, tokens[i] , "urgency")) {
            char *buf = getdata(json, tokens, i);
            ++i;

            if(!is_a_number(buf)) {
                log_err("urgency must be a number");
                free(buf);
                config_cleanup(cfg);
                return NULL;
            }

            cfg->urgency = atoi(buf);
            free(buf);
        }
    }

    free(tokens);
    return cfg;
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

