/*
 *      path.h - Python like path utilities for POSIX complaint systems
 *
 *      TODO: Add description and copyright notice.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "path.h"
#include "debug.h"

enum _pflags{
    p_IR,   // Regular File
    p_ID,   // Directory
    p_IL,   // Link
    p_IF,   // FIFO
    p_E     // Exists
};


static int _run_stat(const char *fname, int flag) {
    int rc = 0;

    // Just some shenanigans to ensure a more consistent call to stat.
    struct stat _s;
    struct stat *s = &_s;

    rc = stat(fname, s);
    
    if(rc && flag != p_E) {
        log_warn("%s", fname);
        // False
        return 0;
    }

    switch(flag) {
        case p_IR:
            return S_ISREG(s->st_mode);
        case p_ID:
            return S_ISDIR(s->st_mode);
        case p_IL:
            return S_ISLNK(s->st_mode);
        case p_IF:
            return S_ISFIFO(s->st_mode);
        case p_E:
            return rc;
        default:
            log_err("Bad flag");
            return 0;
    }
}

int path_isfile(const char *fname) {
    return _run_stat(fname, p_IR);
}

int path_isdir(const char *fname) {
    return _run_stat(fname, p_ID);
}

int path_islink(const char *fname) {
    return _run_stat(fname, p_IL);
}

int path_isfifo(const char *fname) {
    return _run_stat(fname, p_IF);
}

int path_exists(const char *fname) {
    return _run_stat(fname, p_E);
}



