/*
 *
 *
 *      ping.h - ping module
 *
 */

#ifndef __PING_H
#define __PING_H

int _ping_linux(const char *addr, int count);

#ifdef __linux__
#define ping(addr, count) _ping_linux(addr, count)
#endif

#endif
