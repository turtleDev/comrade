/*
 *      debug.h -- debug macros.
 *
 *  This is an adaptation of Zed A. Shaw's debug macro header, from his
 *  book "learn C the hard way".
 */

#ifndef __DBG_H
#define __DBG_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define log_debug(M, ... )
#else
#define log_debug(M, ... ) fprintf(stderr, "[DEBUG: %s(%d):%s()]" M "\n", __FILE__,\
                               __LINE__, __func__, ##__VA_ARGS__)
#endif

#define errno_msg() (errno == 0 ? "":strerror(errno))

#define log_err(M, ... ) fprintf(stderr, "[ERROR: %s(%d):%s()] %s: " M "\n", \
                                 __FILE__, __LINE__, __func__,errno_msg(),\
                                 ##__VA_ARGS__)

#define log_warn(M, ... ) fprintf(stderr, "[WARNING: %s(%d):%s()] %s: " M "\n", \
                                 __FILE__, __LINE__, __func__,errno_msg(),\
                                 ##__VA_ARGS__)

#define log_info(M, ... ) fprintf(stderr, "[INFO: %s(%d):%s()] %s: " M "\n", \
                                 __FILE__, __LINE__, __func__,errno_msg(),\
                                 ##__VA_ARGS__)


#define check(A, M, ... ) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; \
                                    goto error; }

#define sentinel(M, ... ) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
