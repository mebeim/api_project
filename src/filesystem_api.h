#ifndef API_PROJECT_FS_API_INCLUDED
#define API_PROJECT_FS_API_INCLUDED

#define RESULT_SUCCESS      "ok"
#define RESULT_READ_SUCCESS "contenuto"
#define RESULT_FAILURE      "no"

/**
 * Create a file represented by the given path.
 * @param path  : the path representing the file to be created.
 * @param is_dir: whether the file to be created is a directory or not.
 * @post  in case of success the new file is in the proper position in both the hash table and the tree.
 * @out   RESULT_SUCCESS in case of success; RESULT_ERROR in case of error.
 */
void fs_create(char*, bool);

/**
 * Delete the file represented by the given path and, if requested and if any, all its children.
 * @param path     : the path representing the file to be deleted.
 * @param recursive: whether to delete all the file's children (recursively) or not.
 * @post  in case of success, the file has been removed from both the hash table and the tree.
 * @out   RESULT_SUCCESS in case of success; RESULT_ERROR in case of error.
 */
void fs_delete(char*, bool);

/**
 * Output the content of the file represented by the given path.
 * @param path: the path representing the file to read.
 * @out   RESULT_READ_SUCCESS followed by the file's content in case of success; RESULT_ERROR in case of error.
 */
void fs_read(char*);

/**
 * Write the given data to the file represented by the given path.
 * @param path: the path representing the file to write to.
 * @param data: the string to be written to the file.
 * @post  the file contains the given data.
 * @out   RESULT_SUCCESS followed by the length of data in case of success; RESULT_ERROR in case of error.
 */
void fs_write(char*, const char*);

/**
 * Find all the files of the filesystem with the given name.
 * @param name: the name to search for.
 * @out   RESULT_SUCCESS followed by the full path of the matching file, one line per match, sorted lexicographically, in case of success; RESULT_ERROR if no file with the given name is found.
 */
void fs_find(const char*);

/**
 * Destroy the whole filesystem tree (including root) and free all the space.
 * @post the whole filesystem tree and hashtable have been freed.
 */
void fs_exit(void);

#endif