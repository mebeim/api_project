/**
 * File  : utils.h
 * Author: Marco Bonelli
 * Date  : 2017-07-20
 *
 * Copyright (c) 2017 Marco Bonelli.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef API_PROJECT_UTILS_INCLUDED
#define API_PROJECT_UTILS_INCLUDED

#include <stdio.h>

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
