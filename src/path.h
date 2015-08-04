/*
 *      path.h - Python-like path utilities for POSIX complaint platforms
 *
 *      TODO: Add description and copyright notice
 *
 */


int path_isdir(const char *fname);
int path_isfile(const char *fname);
int path_isfifo(const char *fname);
int path_islink(const char *fname);
int path_exists(const char *fname);
