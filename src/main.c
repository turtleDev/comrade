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

#include "debug.h"
#include "config.h"
#include "notification.h"
#include "notification_sound.h"
#include "ping.h"
#include "lock.h"

char * read_file(FILE *f) {
    int count = 0;
    while(fgetc(f) != EOF) ++count;
    rewind(f);
    
    char *str = malloc(sizeof(char) * count);
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

char *DEFAULT_CONFIG = \
"{ \"title\": \"notification\", \
   \"message\": \"your internet is now working\", \
   \"address\": 127.0.0.1, \
   \"ping_count\": 4 }";
        
/* TODO: add usage string */

int main(int argc, char *argv[]) {
    /*TODO: add cmd line option parsing */
    /*TODO: add locking */
    /*TODO: make logging more meaingful */
    /*TODO: add better safety checking */
    /*TODO: TODO FTW! */

    
    char *config_file = "config.json";
    FILE *f;
    struct Config *cfg = NULL;

    f = fopen(config_file, "r+");

    if(!f) {
        log_err("unable to open the file: %s", config_file);
    } else {
        char *config_string = read_file(f);
        fclose(f);
        cfg = config_load(config_string);
    }

    if(!cfg) {
        log_warn("unable to parse configuration file: %s", config_file);
        log_info("using default configuration");
        cfg = config_load(DEFAULT_CONFIG);
    }

    int rc;

    while(1) {
        rc = ping(cfg->address, cfg->ping_count);

        if( rc == 0 ) {
            display_notification(cfg);
            break;
        } else {
            sleep(4);
        }
    }
    config_cleanup(cfg);
    return 0;
}
