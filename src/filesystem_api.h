/**
 * File  : filesystem_api.h
 * Author: Marco Bonelli
 * Date  : 2017-08-14
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

#ifndef API_PROJECT_FS_API_INCLUDED
#define API_PROJECT_FS_API_INCLUDED

#include <stdbool.h>

#define RESULT_SUCCESS      "ok"
#define RESULT_READ_SUCCESS "contenuto"
#define RESULT_FAILURE      "no"

/**
 * Nothing but a wrapper of fs__init: initialize the hash table and create the root.
 * @post the hash table has been allocated in memory and the root has been created.
 */
void fs_init(void);

/**
 * Nothing but a wrapper of fs__exit: destroy the whole filesystem tree (including root) and free all the space.
 * @post the whole filesystem tree and hashtable have been freed.
 */
void fs_exit(void);

/**
 * Create a file represented by the given path.
 * @param path  : the path representing the file to be created.
 * @param is_dir: whether the file to be created is a directory or not.
 * @post  in case of success the new file is in the proper position in both the hash table and the tree.
 * @out   RESULT_SUCCESS in case of success; RESULT_ERROR in case of error.
 */
void fs_create(char* path, bool is_dir);

/**
 * Delete the file represented by the given path and, if requested and if any, all its children.
 * @param path     : the path representing the file to be deleted.
 * @param recursive: whether to delete all the file's children (recursively) or not.
 * @post  in case of success, the file has been removed from both the hash table and the tree.
 * @out   RESULT_SUCCESS in case of success; RESULT_ERROR in case of error.
 */
void fs_delete(char* path, bool recursive);

/**
 * Output the content of the file represented by the given path.
 * @param path: the path representing the file to read.
 * @out   RESULT_READ_SUCCESS followed by the file's content in case of success; RESULT_ERROR in case of error.
 */
void fs_read(char* path);

/**
 * Write the given data to the file represented by the given path.
 * @param path: the path representing the file to write to.
 * @param data: the string to be written to the file.
 * @post  the file contains the given data.
 * @out   RESULT_SUCCESS followed by the length of data in case of success; RESULT_ERROR in case of error.
 */
void fs_write(char* path, const char* data);

/**
 * Find all the files of the filesystem with the given name.
 * @param name: the name to search for.
 * @out   RESULT_SUCCESS followed by the full path of the matching file, one line per match, sorted lexicographically, in case of success; RESULT_ERROR if no file with the given name is found.
 */
void fs_find(const char* name);

#endif
