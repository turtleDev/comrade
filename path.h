/*
 *      path.h - Python-like path utilities for POSIX complaint platforms
 *
 *      TODO: Add description and copyright notice
 *
 */


int path_isdir(char *fname);
int path_isfile(char *fname);
int path_isfifo(char *fname);
int path_islink(char *fname);
int path_exists(char *fname);
