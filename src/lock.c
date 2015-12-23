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
 *      lock.c - File Locking
 *
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __linux__
#include <sys/file.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include "debug.h"
#include "path.h"


/**
 * basename_offset returns the index after the last
 * '/' in a string, or 0 if there weren't any. 
 * This is useful to determine the basename
 * ( that is, files name) is an absolute path
 * string
 *
 * you can determine the base name of an absolute
 * path like this:
 * char *s = as + basname_offset(as);
 *
 * if the path is not an abolsute path, that works
 * too, because then it returns 0.
 */
static int basename_offset(char *string)
{
    int i;
    char target = '\0';

#if defined(__linux__)
    target = '/';
#elif defined(_WIN32)
    target = '\\';
#endif

    for (i = strlen(string)-1; i > 0; --i) {
        if ( string[i] == target ) {
            return i+1;
        }
    }
    return i;
}

/* Linux Specific Locking */

#ifdef __linux__
static FILE *lf = NULL;
static char *linux_lockfile = NULL;

/**
 * _lock_acquire_linux() acquires an advisory lock on a file
 * by the name PROGRAMNAME-USERNAME.lock in the /tmp directory.
 * 
 * You can use this function to ensure that only one instance of
 * a program runs per user. 
 *
 * @param program_name name of the program ( argv[0] )
 * 
 * @returns 0 for success, -1 for error (may set errno).
 */
int _lock_acquire_linux(char *program_name) {

    char *tmp_dir = NULL;
    char *user = getenv("USER");
    
    // see basename_custom 
    char *base = program_name + basename_offset(program_name);
    
    if(P_tmpdir) {
        tmp_dir = strdup("/tmp");
    }

    check(path_isdir(tmp_dir), "unable to create lockfile");
    
    // remove any slash at the end
    if(tmp_dir[strlen(tmp_dir) -1] == '/') {
        tmp_dir[strlen(tmp_dir) -1] = '\0';
    }
    
    int count = strlen(tmp_dir) + 
                strlen(base) +
                strlen(user) +
                strlen(".lock") +
                5; // Just as a precaution; 

    linux_lockfile = malloc(sizeof(char) * count);
    check(linux_lockfile, "Out of memory");

    sprintf(linux_lockfile,"%s/%s-%s.lock",
             tmp_dir, base, user);


    lf = fopen(linux_lockfile, "w");

    check(lf, "unable to open %s for acquiring lock", linux_lockfile);
    
    int rc = flock(fileno(lf), LOCK_EX | LOCK_NB);
               
    free(tmp_dir);
    
    return rc;

// check() causes control to jump here, if the condition is false
error:
    if (tmp_dir) free(tmp_dir);
    if (linux_lockfile) free(linux_lockfile);
    return -1;
}

/**
 * _lock_release_linux(void) release the advisory lock acquired by
 * _lock_acquire_linux().
 *
 * @returns 0 for success, -1 for error.
 */
int _lock_release_linux(void) {
    int rc = flock(fileno(lf), LOCK_UN);
    fclose(lf);
    if(linux_lockfile) {
        remove(linux_lockfile); 
    }
    free(linux_lockfile);
    linux_lockfile = NULL;
    return rc;
}

#endif /* end __linux__ */

/* windows specific locking  */
#ifdef _WIN32

HANDLE mutex_obj;

int _lock_acquire_win(char *program_name) {
    char *user = getenv("USERNAME");
    char *base_name = program_name + basename_offset(program_name);
    /**
     * compute length required for the string 
     * PROGRAMNAME-USERNAME + the null terminating
     * byte
     */
    int len = strlen(user) + strlen(base_name) + 2;

    
    char mutex_name[len];
    sprintf(mutex_name, "%s-%s",base_name, user);

    /* try acquring the lock */
    mutex_obj = CreateMutexA(
                       NULL,      /* default security settings */
                       TRUE,      /* acquire lock on this object */
                       mutex_name /* name of the mutex */
                   );

    if ( !mutex_obj ||
         GetLastError() == ERROR_ALREADY_EXISTS || 
         GetLastError() == ERROR_ACCESS_DENIED ) {
        return -1;
    } else {
        return 0;
    }
}

int _lock_release_win(void) {
    return ReleaseMutex(mutex_obj);
}

#endif /* end _WIN32 */
