#include <stdio.h>

#ifndef API_PROJECT_UTILS_INCLUDED
#define API_PROJECT_UTILS_INCLUDED

#define GETDELIMS_BUFFER_CHUNK_SIZE 128

/**
 * Wrapper of malloc which forces the program to exit in case of failure.
 */
void* malloc_or_die(size_t sz);

/**
 * Wrapper of calloc which forces the program to exit in case of failure.
 */
void* calloc_or_die(size_t n, size_t sz);

/**
 * Wrapper of realloc which forces the program to exit in case of failure.
 */
void* realloc_or_die(void* mem, size_t sz);

/**
 * Wrapper of malloc_or_die/calloc_or_die which allocates an array of NULL pointers.
 */
void* malloc_null(size_t n, size_t sz);

/**
 * Read from the provided stream into the provided string variable until any delimiter or EOF is found, dynamically allocating the needed memory.
 * @param str   : address of the array of chars where to put the string read.
 * @param delims: string containing all the delimiter characters.
 * @param stream: file stream to read from.
 * @ret   number of characters read or -1 in case EOF was already reached.
 * @post  *str contains the NUL-terminated string read.
 */
int getdelims(char** restrict str, const char* delims, FILE* restrict stream);

#endif
