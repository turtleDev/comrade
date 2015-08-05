/*
 *
 *
 *      config.c - configuration module
 *
 *      TODO: add description and copyright notice
 */

#include <stdlib.h>
#include <string.h>

#include "debug.h"

#ifdef __linux__
#include "include/jsmn.h"
#endif

#include "config.h"

/* TODO: include headers using windows path on windows platform */

int isequal(const char *json, jsmntok_t t, const char *s) {
   if(t.type == JSMN_STRING && (strlen(s) == (t.end - t.start)) &&
      !strncmp(json + t.start, s, t.end - t.start)) { 
        return 1;
    }
   return 0;
}

char * getdata(const char *json, jsmntok_t t[], int i) {
    char *str = strndup(json + t[i+1].start,t[i+1].end - t[i+1].start);
    return str;
}
    

struct Config * config_load(const char *json) {
    int rc;
    jsmn_parser parser;

    jsmntok_t *tokens = NULL;

    jsmn_init(&parser);

    // parse the number of tokens.
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

    cfg->title = NULL;
    cfg->message = NULL;
    cfg->sound_file = NULL;
    cfg->icon_file = NULL;

    int i;
    // we're only going to parse the data inside the first object.
    int len = tokens[0].size * 2;
    // I start the index from one, since the index 0 is the toplevel object
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
        free(cfg);
    }
}

