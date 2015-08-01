/*
 *      lock.c - File Locking
 *
 *      TODO: Add description and copyright notice.
 *
 */


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"

// --- Linux Specific Locking ---

static FILE *lf = NULL;
static char *linux_lockfile = NULL;

int _lock_acquire_linux(char *program_name) {

    char *tmp_dir = NULL;
    char *user = getenv("USER");
    
    if(P_tmpdir) {
        tmp_dir = strdup(P_tmpdir);
    } else {
        tmp_dir = strdup("/tmp");
    }

    check(path_isdir(tmp_dir), "unable to create lockfile");
    
    // remove any slash at the end
    if(tmp_dir[strlen(tmp_dir) -1] == '/') {
        tmp_dir[strlen(tmp_dir) -1] = '\0';
    }
    
    int count = strlen(tmp_dir) + 
                strlen(program_name) +
                strlen(user) +
                strlen(".lock") +
                5; // Just as a precaution;

    linux_lockfile = malloc(sizeof(char) * count);
    check(linux_lockfile, "Out of memory");

    sprintf(linux_lockfile,"%s/%s-%s.lock",
             tmp_dir, program_name, user);


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
