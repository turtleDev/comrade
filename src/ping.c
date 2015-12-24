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
 *      ping.c - ping module
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ping.h"


/**
 * helper. Compute the `size` of an integer.
 * like 100 is 3
 */
static int compute_size(int n) {
    int s = 1;
    while ( n != 0 ) 
        n /= 10;
    return s;
}

/**
 * a wrapper around itoa() that mallocs memory
 * for the string.
 *
 * returns the converted number as freshly allocated
 * string.
 */
static char *str_from_int(int val) {

    int size = compute_size(val) +1;

    char *buffer = (char *)malloc(sizeof(char) * size);

    itoa(val, buffer, 10);

    return buffer;
}

#if defined(__linux__)

int ping(const char *addr, int count) {

    char *count = str_from_int(count);
    char *cmd_template = "ping %s -c %s 2>&1";

    /**
     * compute the length of the buffer required to hold
     * the command that we want to execute
     */
    int len = strlen(addr) + strlen(cmd_template) + strlen(count);

    char *cmd = malloc(sizeof(char) * len);

    if(count <= 0) {
        count = 1;
    }
    
    sprintf(cmd, cmd_template, addr, count);

    FILE *s = popen(cmd, "r");
    char ch;

    // empty the stream
    while((ch=fgetc(s)) != EOF);

    free(cmd);
    free(count);

    return pclose(s);
}

#elif defined(_WIN32)

int ping(const char *addr, int count ) {

    char *cmd_template = "ping %s -n %s > NUL";
    char *count = str_from_int(count);

    int len = strlen(cmd_template) + strlen(count) + strlen(addr) +1;

    sprintf(cmd, cmd_template, addr, count);

    /**
     * since popen() is not available for GUI applications(Thank you MS)
     * use system()
     */
    int rc = system(cmd);

    free(count);
    free(cmd);

    return rc;
}

#else 

/**
 * stub
 */

int ping(const char *addr, int count ) {
    log_warn("stub: dummy ping");
    return 0;
}


