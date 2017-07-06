#ifndef API_PROJEXT_FS_CORE_INCLUDED
#define API_PROJEXT_FS_CORE_INCLUDED

#define MAX_FILESYSTEM_DEPTH 255
#define MAX_DIRECTORY_CHILDREN 1024

/**
 * Hash the string provided as key (adapted from the djb2 hash function by Dan Bernstein).
 * @param key: the string to be hashed.
 * @ret   the computed hash.
 */
unsigned long djb2(const char*);

/**
 * Scan the table from a start index until a valid cell is found.
 * @param start : starting index.
 * @param key   : file name to match which was used as key to produce the initial hash.
 * @param parent: file parent to match.
 * @param new   : whether to search for a new (empty) cell or an existing file.
 * @ret   index of the wanted cell in the table, -1 if it doesn't exist.
 * @pre   start has been created as start = (parent->hash + djb2(key)) % fs_table_size.
 */
int linear_probe(size_t, const char*, const fs_file_t*, bool);

/**
 * Destroy the table and allocate a new one with double size, rehashing all the files.
 * @post fs_table_t is double its previous size and contains all the files, fs_table_size contains the new size.
 */
void expand_table(void);

/**
 * Create a new file, initialize it according to the given parameters and insert it in the list of its parent's children.
 * @param hash  : the hash of the new file.
 * @param name  : the name of the new file.
 * @param is_dir: whether the new file is a directory or not.
 * @param parent: a pointer to the new file's parent.
 * @ret   a pointer to the new file.
 * @pre   all the checks before the creation have already been made.
 * @post  the new file is now the head of the list of children starting at parent->content.l_child.
 */
fs_file_t* fs__new(size_t, char*, bool, fs_file_t*);

/**
 * Browse the filesystem following the path and return a pointer to the table cell identified by the path.
 * @param path      : the path of the file to get.
 * @param parent_arg: reference to a pointer to file, where the parent's address will be stored if needed, NULL otherwise.
 * @param new       : whether the path refers to a new file or an already existing one.
 * @ret   a pointer to the requested table cell or NULL in case of an error (e.g. a folder in the path doesn't exist).
 * @pre   parent_arg is either NULL (if the caller doesn't want to know the parent) or the address of a pointer to file.
 * @post  if it wasn't NULL, parent_arg contains the addres of the hash table cell containing the parent.
 */
fs_file_t** fs__get(char*, fs_file_t**, bool);

/**
 * Search all the files with the given name starting from cur and exploring the tree recursively.
 * @param cur : pointer to the file from which the search will start.
 * @param name: the name to search.
 * @param n   : reference to a counter where the number of matches will be stored.
 * @ret   an array of pointers to files which all have the same requested name.
 * @pre   cur is a valid file pointer (not NULL).
 */
fs_file_t** fs__all(fs_file_t*, const char*, size_t*);

/**
 * Trace the given file back until the root and return its full path.
 * @param cur: the file of which the path is requested.
 * @param len: the length of the path in characters, should be 0 (zero) for the first recursive call.
 * @ret   a string representing the full path to the file.
 * @pre   cur is a valid file pointer (not NULL).
 */
char* fs__uri(fs_file_t*, size_t);

/**
 * Delete cur and all the files contained in its subtree exploring it recursively.
 * @param cur: address of a pointer to the file to delete.
 * @pre   cur must be the address of a ->content.l_child or ->r_sibling field of an existing file.
 * @post  the requested file and all its children have been deleted; their cells in the hash table contain the value FS_DELETED.
 * @pre   cur is the address of a valid file pointer (not NULL or referencing NULL).
 */
void fs__del(fs_file_t**);

/**
 * Compare two paths using strcmp and return the result. Used as compare function for qsort.
 * @param a: first path to compare.
 * @param b: second path to compare.
 * @ret   <>= 0 wether a comes before b, after b or is equal to b.
 */
int fs__cmp(const void*, const void*);

#endif