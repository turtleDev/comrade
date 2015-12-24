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

/**
 * lock_acquire() acquires lock on a user local
 * resource. This call ensures that only one instance
 * of a program runs per user. 
 *
 * Do not call this more than once per program.
 *
 * @param program_name name of the program
 *
 * @returns 0 for success, -1 for error (may set errno)
 */
int lock_acquire(char *program_name);

/**
 * lock_release() releases the resource lock acquired by
 * lock_acquire(). 
 *
 * @returns 0 for success, and -1 for failure.
 */
int lock_release(void);

#endif /* end __LOCK__H */
