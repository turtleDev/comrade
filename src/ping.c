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

int _ping_linux(const char *addr, int count) {

    // 32 bytes are extra padding, just in case.
    int len = strlen(addr) + strlen("ping ") + 32;
    char *cmd = malloc(sizeof(char) * len);

    if(count <= 0) {
        count = 1;
    }
    
    sprintf(cmd, "ping %s -c %d 2>&1", addr, count);

    FILE *s = popen(cmd, "r");
    char ch;

    // empty the stream
    while((ch=fgetc(s)) != EOF);

    free(cmd);

    return pclose(s);
}

