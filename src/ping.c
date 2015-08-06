/*
 *
 *
 *      ping.c - ping module
 *
 *      TODO: add description and copyright notice.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ping.h"

int _ping_linux(const char *addr, int count) {

    // 16 bytes are extra padding, just in case.
    int len = strlen(addr) + strlen("ping ") + 16;
    char *cmd = malloc(sizeof(char) * len);
    
    sprintf(cmd, "ping %s -c %d 2>&1", addr, count);

    FILE *s = popen(cmd, "r");
    char ch;

    // empty the stream
    while((ch=fgetc(s)) != EOF);

    free(cmd);

    return pclose(s);
}

