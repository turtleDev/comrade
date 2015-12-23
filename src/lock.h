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
 *      lock.h -- File locking
 *
 */

#ifndef __LOCK_H
#define __LOCK_H


/* forward declarations */
#ifdef __linux__

int _lock_acquire_linux(char *program_name);
int _lock_release_linux(void);

#elif _WIN32

int _lock_acquire_win(char *program_name);
int _lock_release_win();

#endif

/* helper macros */
#ifdef __linux__

#define lock_acquire(program_name) _lock_acquire_linux(program_name)
#define lock_release() _lock_release_linux()

#endif

#ifdef _WIN32

#define lock_acquire(program_name) _lock_acquire_win(program_name)
#define lock_release() _lock_release_win()

#endif

#endif /* end __LOCK__H */
