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
 *      debug.h - debug macros.
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
#define log_debug(M, ... ) fprintf(stderr, "[DEBUG: %s(%d):%s()] " M "\n" , __FILE__,\
                               __LINE__, __func__, ##__VA_ARGS__)
#endif

#define errno_msg() (errno == 0 ? "":strerror(errno))

#define log_err(M, ... ) fprintf(stderr, "[ERROR: %s(%d):%s()] " M " (%s)\n", \
                                 __FILE__, __LINE__, __func__,\
                                 ##__VA_ARGS__,errno_msg())

#define log_warn(M, ... ) fprintf(stderr, "[WARNING: %s(%d):%s()] " M " (%s)\n", \
                                 __FILE__, __LINE__, __func__,\
                                 ##__VA_ARGS__, errno_msg())

#define log_info(M, ... ) fprintf(stderr, "[INFO: %s(%d):%s()] " M " (%s)\n", \
                                 __FILE__, __LINE__, __func__,\
                                 ##__VA_ARGS__, errno_msg())


#define check(A, M, ... ) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; \
                                    goto error; }

#define sentinel(M, ... ) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
