#ifndef API_PROJECT_FS_CORE_INCLUDED
#define API_PROJECT_FS_CORE_INCLUDED

#define MAX_FILESYSTEM_DEPTH 255
#define MAX_DIRECTORY_CHILDREN 1024

typedef union  fs_file_content_u fs_file_content_t;
typedef struct fs_file_s         fs_file_t;

union fs_file_content_u {
	fs_file_t* l_child;
	char* data;
};

struct fs_file_s {
	size_t hash;
	char* name;
	bool is_dir;
	unsigned short n_children;
	fs_file_content_t content;
	fs_file_t *parent, *l_sibling, *r_sibling;
};

fs_file_t** fs_table;
fs_file_t*  fs_root;
unsigned    fs_table_files;
size_t      fs_table_size;

/**
 * Initialize the hash table and create the root.
 * @post the hash table has been allocated in memory and the root has been created.
 */
void fs__init(void);

/**
 * Destroy the whole filesystem tree (including root) and free all the space.
 * @post the whole filesystem tree and hashtable have been freed.
 */
void fs__exit(void);

/**
 * Create a new file, initialize it according to the given parameters and insert it in the list of its parent's children.
 * @param name  : the name of the new file.
 * @param is_dir: whether the new file is a directory or not.
 * @param parent: a pointer to the new file's parent.
 * @ret   a pointer to the new file.
 * @pre   all the checks before the creation have already been made.
 * @post  the new file is now the head of the list of children starting at parent->content.l_child.
 */
fs_file_t* fs__new(char* name, bool is_dir, fs_file_t* parent);

/**
 * Browse the filesystem following the path and return a pointer to the table cell identified by the path, creating a new file in such cell if requested.
 * @param path      : the path of the file to get.
 * @param new       : whether the path refers to a new file or an already existing one.
 * @param new_is_dir: whether the new file is a directory or not.
 * @ret   a pointer to the requested table cell or NULL in case of an error (e.g. a folder in the path doesn't exist).
 * @post  if new is true, a new file is created in the table cell identified by the path.
 */
fs_file_t** fs__get(char* path, bool new, bool new_is_dir);

/**
 * Search all the files with the given name starting from cur and exploring the tree recursively.
 * @param cur : pointer to the file from which the search will start.
 * @param name: the name to search.
 * @param n   : reference to a counter where the number of matches will be stored.
 * @ret   an array of pointers to files which all have the same requested name.
 * @pre   cur is a valid file pointer (not NULL).
 */
fs_file_t** fs__all(fs_file_t* cur, const char* name, size_t* n);

/**
 * Trace the given file back until the root and return its full path.
 * @param cur: the file of which the path is requested.
 * @param len: the length of the path in characters, should be 0 (zero) for the first recursive call.
 * @ret   a string representing the full path to the file.
 * @pre   cur is a valid file pointer (not NULL).
 */
char* fs__uri(fs_file_t* cur, size_t len);

/**
 * Delete cur and all the files contained in its subtree exploring it recursively.
 * @param cur: address of a pointer to the file to delete.
 * @pre   cur must be the address of a ->content.l_child or ->r_sibling field of an existing file.
 * @post  the requested file and all its children have been deleted; their cells in the hash table contain the value FS_DELETED.
 * @pre   cur is the address of a valid file pointer (not NULL or referencing NULL).
 */
void fs__del(fs_file_t** cur);

/**
 * Compare two paths using strcmp and return the result. Used as compare function for qsort.
 * @param a: first path to compare.
 * @param b: second path to compare.
 * @ret   <>= 0 wether a comes before b, after b or is equal to b.
 */
int fs__cmp(const void* a, const void* b);

#endif