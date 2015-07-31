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

// I'm assuming that tmpdir will generally be /tmp or /var/tmp
// so I'm only going to allocate 512 bytes for lockfile buffer.
static const int LOCKFILE_PATH_LIMIT = 512;
static char linux_lockfile[512] = {'\0'};
static FILE *lf = NULL;

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
    
    int count = snprintf(linux_lockfile, LOCKFILE_PATH_LIMIT,"%s/%s-%s.lock",
                         tmp_dir, program_name, user);

    check(count < LOCKFILE_PATH_LIMIT, "path too long!");

    
    lf = fopen(linux_lockfile, "w");

    check(lf, "unable to open %s for acquiring lock", linux_lockfile);
    
    int rc = flock(fileno(lf), LOCK_EX | LOCK_NB);
               
    free(tmp_dir);
    
    return rc;

// check() causes control to jump here, if the condition is false
error:
    if (tmp_dir) free(tmp_dir);
    return -1;
}

int _lock_release_linux(void) {
    int rc = flock(fileno(lf), LOCK_UN);
    fclose(lf);
    if(linux_lockfile) {
        remove(linux_lockfile); 
    }
    return rc;
}
