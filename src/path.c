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
 *      path.h - Python like path utilities for POSIX complaint systems
 */

#include <unistd.h>
#include <stdlib.h>

/* platform specific headers */

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __linux__

#include <wordexp.h>

#endif /* end __linux__ */


#include "path.h"
#include "debug.h"


#ifdef _WIN32

/**
 * This function issues a warning when a non-supported feature is
 * queried. The call itself succeeds, but this might result in bugs
 * if the caller does not know about it.
 * in the future, I might change it to crash the application when
 * a non supported feature is requested.
 */
static int _warn_not_supported(char *feature, int n) {
	log_warn("%s is not supported on Microsoft Windows", feature);
	return n;
}

/**
 * stat on windows cannot determine whether a file a link
 * or not. This a dummy macro that make porting code a litte
 * easier from linux to windows. 
 *
 * However, see the note on _warn_not_supported() 
 */
#define S_ISLNK(n) (_warn_not_supported("path_islink()", n))

#endif 

/**
 *  ghost macro 
 *
 *  The following code was originally used back when comrade was being 
 *  developed using Visual Studio. It's useless now since
 *  mingw has the proper definitions for stat and its
 *  macros.
 */
#ifdef VC_LEGACY

/* the struct stat*/
#define stat _stat

/* the function stat */
#define stat(f, st) _stat(f, st)

/* helper macros */
#define S_ISREG(n) (_S_IFREG & n)
#define S_ISDIR(n) (_S_IFDIR & n)
#define S_ISFIFO(n) (_S_IFIFO & n)

#endif /* end _WIN32 */


enum _pflags{
    p_IR,   // Regular File
    p_ID,   // Directory
    p_IL,   // Link
    p_IF,   // FIFO
    p_E     // Exists
};


static int _run_stat(char *fname, int flag) {
    int rc = 0;

    // Just some shenanigans to ensure a more consistent call to stat.
    struct stat _s;
    struct stat *s = &_s;

#ifdef __linux__
    wordexp_t result;
    int rv = wordexp(fname, &result, 0);

    if ( !rv ) {
        /**
         * Comrade only really cares for HOME expansion.
         */
        fname = result.we_wordv[0];
    }

#endif /* end __linux__ */

    rc = stat(fname, s);

#ifdef __linux__
    /**
     * now that we have no use for the file path,
     * we can free this struct
     */
    wordfree(&result);
#endif /* end __linux__ */

    
    if(rc && flag != p_E) {
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
    return _run_stat( (char *) fname, p_IR);
}

int path_isdir(const char *fname) {
    return _run_stat( (char *) fname, p_ID);
}

int path_islink(const char *fname) {
    return _run_stat( (char *) fname, p_IL);
}

int path_isfifo(const char *fname) {
    return _run_stat( (char *) fname, p_IF);
}

int path_exists(const char *fname) {
    return _run_stat( (char *) fname, p_E);
}


#if defined(__linux__)

int path_isabsolute(const char *path) {
    return path[0] == '/';
}

int path_isrelative(const char *path) {
    return !path_isabsolute(path);
}

#elif defined(_WIN32)

int path_isabsolute(const char *path) {
    log_warn("stub: not implemented");
    return 0;
}

int path_isrelative(const char *path) {
    log_warn("stub: not implemented");
    return 0;
}

#endif
