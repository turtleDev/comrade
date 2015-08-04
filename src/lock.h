/*
 *      lock.h -- File locking
 *
 *      TODO: add copyright notice
 */

#ifndef _lock_h
#define _lock_h

//  --- Linux specific locks ---
int _lock_acquire_linux(char *program_name);
int _lock_release_linux(void);

#ifdef __linux__
 
#define lock_acquire(f) _lock_acquire_linux(f)
#define lock_release() _lock_release_linux()

#endif

#endif
