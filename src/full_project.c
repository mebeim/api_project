/**
 * Subject: Final test project
 * Author : Marco Bonelli
 * Date   : 2017-07-05
 * Course : Algorithms and principles of computer science [ID:086067]
 * A.Y.   : 2016/2017
 *
 * (c) Marco Bonelli 2017
 * Licensed under the Apache License 2.0.
 * https://github.com/mebeim/api_project
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define GETDELIMS_BUFFER_CHUNK_SIZE 128
#define MAX_FILESYSTEM_DEPTH        255
#define MAX_DIRECTORY_CHILDREN      1024

#define RESULT_SUCCESS      "ok"
#define RESULT_READ_SUCCESS "contenuto"
#define RESULT_FAILURE      "no"

#define COMMAND_CREATE 'c'
#define COMMAND_DELETE 'd'
#define COMMAND_READ   'r'
#define COMMAND_WRITE  'w'
#define COMMAND_FIND   'f'
#define COMMAND_EXIT   'e'

/****************************************************
 *                 TYPE DEFINITIONS                 *
 ****************************************************/

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

/****************************************************
 *                FUNCTION PROTOTYPES               *
 ****************************************************/

/* Global wrappers/helpers */
static inline void* malloc_or_die  (size_t);
static inline void* calloc_or_die  (size_t, size_t);
static inline void* realloc_or_die (void*, size_t);
static inline void* malloc_null    (size_t, size_t);
              int   getdelims      (char** restrict, const char*, FILE* restrict);

/* Hash & hash table functions */
unsigned long djb2         (const char*);
int           linear_probe (size_t, const char*, const fs_file_t*, bool);
void          rehash_all   (fs_file_t*);
void          expand_table (void);

/* Filesystem helpers */
fs_file_t*  fs__new(char*, bool, fs_file_t*);
fs_file_t** fs__get(char*, bool, bool);
fs_file_t** fs__all(fs_file_t*, const char*, size_t*);
char*       fs__uri(fs_file_t*, size_t);
void        fs__del(fs_file_t**);
int         fs__cmp(const void*, const void*);

/* Filesystem functions */
void fs_create (char*, bool);
void fs_delete (char*, bool);
void fs_read   (char*);
void fs_write  (char*, const char*);
void fs_find   (const char*);
void fs_exit   (void);

/****************************************************
 *                 GLOBAL VARIABLES                 *
 ****************************************************/

fs_file_t* const FS_DELETED        = (fs_file_t*) -1;
float      const FS_TABLE_MAX_LOAD = 2.0 / 3.0;
size_t     const FS_ROOT_HASH      = 0;
char*      const FS_ROOT_NAME      = "#";

fs_file_t** fs_table;
fs_file_t*  fs_root;
unsigned    fs_table_files;
size_t      fs_table_size;

/****************************************************
 *                       MAIN                       *
 ****************************************************/

int main(void) {
	char *line, *cmd, *arg, *str;
	int chars_read;
	bool done;

	fs_table_files = 0;
	fs_table_size  = 1024 * 1024 / sizeof(fs_file_t*);
	fs_table       = malloc_null(fs_table_size, sizeof(fs_file_t*));
	fs_root        = fs__new(NULL, true, NULL);
	done           = false;

	while (!done) {
		chars_read = getdelims(&line, "\r\n", stdin);

		if (chars_read == 0) {
			free(line);
			continue;
		}

		if (chars_read == -1) {
			free(line);
			break;
		}

		cmd = strtok(line, " \t");
		arg = strtok(NULL, " \t\r\n");

		switch (cmd[0]) {
			case COMMAND_CREATE:
				fs_create(arg, (bool)cmd[6]);
				break;

			case COMMAND_DELETE:
				fs_delete(arg, (bool)cmd[6]);
				break;

			case COMMAND_READ:
				fs_read(arg);
				break;

			case COMMAND_WRITE:
				str = strtok(NULL, "\"");
				fs_write(arg, str);
				break;

			case COMMAND_FIND:
				fs_find(arg);
				break;

			case COMMAND_EXIT:
				done = true;
				fs_exit();
				break;
		}

		free(line);
	}

	return 0;
}

/****************************************************
 *              GLOBAL WRAPPERS/HELPERS             *
 ****************************************************/

/**
 * Wrapper of malloc which forces the program to exit in case of failure.
 */
static inline void* malloc_or_die(const size_t sz) {
	void* mem;

	mem = malloc(sz);
	if (mem == NULL)
		exit(1);

	return mem;
}

/**
 * Wrapper of calloc which forces the program to exit in case of failure.
 */
static inline void* calloc_or_die(size_t n, size_t sz) {
	void* mem;

	mem = calloc(n, sz);
	if (mem == NULL)
		exit(1);

	return mem;
}

/**
 * Wrapper of realloc which forces the program to exit in case of failure.
 */
static inline void* realloc_or_die(void* mem, size_t sz) {
	mem = realloc(mem, sz);
	if (mem == NULL)
		exit(1);

	return mem;
}

/**
 * Wrapper of malloc_or_die/calloc_or_die which allocates an array of NULL pointers.
 */
static inline void* malloc_null(size_t n, size_t sz) {
	void** mem;
	size_t i;

	if (0 == NULL) {
		mem = calloc_or_die(n, sz);
	} else {
		mem = malloc_or_die(sz * n);
		for (i = 0; i < n; i++)
			mem[i] = NULL;
	}

	return mem;
}

/**
 * Read from the provided stream into the provided string variable until any delimiter or EOF is found, dynamically allocating the needed memory.
 * @param str   : address of the array of chars where to put the string read.
 * @param delims: string containing all the delimiter characters.
 * @param stream: file stream to read from.
 * @ret   number of characters read or -1 in case EOF was already reached.
 * @post  *str contains the NUL-terminated string read.
 */
int getdelims(char** restrict str, const char* delims, FILE* restrict stream) {
	size_t str_size, str_len;
	register int c;
	char* cur;

	str_len  = 0;
	str_size = GETDELIMS_BUFFER_CHUNK_SIZE + 1;
	*str     = malloc_or_die(str_size);
	cur      = *str;

	for (;;) {
		c = getc(stream);

		if (str_len == str_size) {
			str_size += GETDELIMS_BUFFER_CHUNK_SIZE;
			*str      = realloc_or_die(*str, str_size);
			cur       = *str + str_len;
		}

		if (strchr(delims, c) != NULL)
			break;

		if (c == EOF) {
			if (str_len == 0)
				return -1;
			break;
		}

		*cur = c;
		cur++;
		str_len++;
	}

	*cur = '\0';

	return (int)str_len;
}

/****************************************************
 *            HASH & HASH TABLE FUNCTIONS           *
 ****************************************************/

/**
 * Hash the string provided as key (adapted from the djb2 hash function by Dan Bernstein).
 * @param key: the string to be hashed.
 * @ret   the computed hash.
 */
unsigned long djb2(const char* key) {
	unsigned char* k;
	unsigned long h;
	int c;

	h = 5381;
	k = (unsigned char*)key;

	while ((c = *k++))
		h = (h << 5) + h + c;

	return h;
}

/**
 * Scan the table from a start index until a valid cell is found.
 * @param start : starting index.
 * @param key   : file name to match which was used as key to produce the initial hash.
 * @param parent: file parent to match.
 * @param new   : whether to search for a new (empty) cell or an existing file.
 * @ret   index of the wanted cell in the table, -1 if it doesn't exist.
 * @pre   start has been created as start = (parent->hash + djb2(key)) % fs_table_size.
 */
int linear_probe(size_t start, const char* key, const fs_file_t* parent, bool new) {
	register size_t h;

	h = start;

	if (new) {
		// While the cell isn't empty:
		while (fs_table[h] != NULL && fs_table[h] != FS_DELETED) {
			// If both name and parent of the file in the current cell match the given ones:
			if (fs_table[h]->parent == parent && strcmp(fs_table[h]->name, key) == 0)
				// Then the file we want to create already exists.
				return -1;
			// Otherwise keep going on:
			h = (h + 1) % fs_table_size;
		}
	} else {
		// While the cell is not empty and it's either a tombstone or the name or parent don't match the given ones:
		while (fs_table[h] != NULL && (fs_table[h] == FS_DELETED || fs_table[h]->parent != parent || strcmp(fs_table[h]->name, key) != 0))
			// Keep going on:
			h = (h + 1) % fs_table_size;

		// If we stopped because the cell is empty:
		if (fs_table[h] == NULL)
			// Then the file we're looking for doesn't exist.
			return -1;
	}

	// If none of the above return statements executed, we found the right cell.
	return (int)h;
}

/**
 * Rehash the files starting from cur and exploring the tree recursively.
 * @param cur: pointer to the file to rehash.
 * @pre   cur is a valid file pointer (not NULL) which was already properly removed from the table.
 * @post  cur->hash is the new hash and new position in the table.
 */
void rehash_all(fs_file_t* cur) {
	fs_file_t* child;

	if (cur->parent != NULL) {
		// Rehash the current file and put it back in the table:
		cur->hash = (cur->parent->hash + djb2(cur->name)) % fs_table_size;
		cur->hash = linear_probe(cur->hash, cur->name, cur->parent, true);
		fs_table[cur->hash] = cur;
	}

	if (cur->is_dir) {
		// Rehash all the children:
		child = cur->content.l_child;
		while (child != NULL) {
			rehash_all(child);
			child = child->r_sibling;
		}
	}
}

/**
 * Destroy the table and allocate a new one with double size, rehashing all the files.
 * @post fs_table_t is double its previous size and contains all the files, fs_table_size contains the new size.
 */
void expand_table(void) {
	free(fs_table);
	fs_table_size *= 2;
	fs_table = malloc_null(fs_table_size, sizeof(fs_file_t*));
	rehash_all(fs_root);
}

/****************************************************
 *                FILESYSTEM HELPERS                *
 ****************************************************/

/**
 * Create a new file, initialize it according to the given parameters and insert it in the list of its parent's children.
 * @param name  : the name of the new file.
 * @param is_dir: whether the new file is a directory or not.
 * @param parent: a pointer to the new file's parent.
 * @ret   a pointer to the new file.
 * @pre   all the checks before the creation have already been made.
 * @post  the new file is now the head of the list of children starting at parent->content.l_child.
 */
fs_file_t* fs__new(char* name, bool is_dir, fs_file_t* parent) {
	fs_file_t* new;

	// Before creating a new file, expand the hash table if the maximum load coefficent has been exceeded:
	if (((float)fs_table_files / (float)fs_table_size) > FS_TABLE_MAX_LOAD)
		expand_table();

	new             = malloc_or_die(sizeof(fs_file_t));
	new->is_dir     = is_dir;
	new->n_children = 0;
	new->parent     = parent;

	if (is_dir)
		new->content.l_child = NULL;
	else
		new->content.data = calloc_or_die(1, sizeof(char));

	// If we are creating the root:
	if (parent == NULL) {
		// Its hash is FS_ROOT_HASH:
		new->hash      = FS_ROOT_HASH;
		// Its name is FS_ROOT_NAME
		new->name      = FS_ROOT_NAME;
		// And it hasn't got siblings.
		new->l_sibling = NULL;
		new->r_sibling = NULL;
	} else {
		// Otherwise calculate the new file's hash:
		new->hash      = (parent->hash + djb2(name)) % fs_table_size;
		new->hash      = linear_probe(new->hash, name, parent, true);
		// Use the provided name as the new file's name:
		new->name      = malloc_or_die(strlen(name) + 1);
		strcpy(new->name, name);

		// And insert the new file in the head of the list of children of its parent:
		new->l_sibling = NULL;
		new->r_sibling = parent->content.l_child;
		if (new->r_sibling != NULL)
			new->r_sibling->l_sibling = new;

		parent->content.l_child = new;
		parent->n_children++;
	}

	return new;
}

/**
 * Browse the filesystem following the path and return a pointer to the table cell identified by the path, creating a new file in such cell if requested.
 * @param path      : the path of the file to get.
 * @param new       : whether the path refers to a new file or an already existing one.
 * @param new_is_dir: whether the new file is a directory or not.
 * @ret   a pointer to the requested table cell or NULL in case of an error (e.g. a folder in the path doesn't exist).
 * @post  if new is true, a new file is created in the table cell identified by the path.
 */
fs_file_t** fs__get(char* path, bool new, bool new_is_dir) {
	fs_file_t** parent;
	register unsigned short depth;
	char *cur_name, *next_name;
	int cur_hash;

	depth     = 0;
	parent    = &fs_root;
	cur_name  = strtok(path, "/");
	next_name = strtok(NULL, "/");
	cur_hash  = FS_ROOT_HASH;

	// While the parent exists, we're not at the last file name in the path and we didn't reach the maximum filesystem depth:
	while (   parent    != NULL
	       && *parent   != NULL
	       && *parent   != FS_DELETED
	       && cur_name  != NULL
	       && next_name != NULL
		   && depth      < MAX_FILESYSTEM_DEPTH
	) {
		// If the parent hasn't got children:
		if (!((*parent)->n_children > 0))
			// The directory we're looking for certainly doesn't exist.
			return NULL;

		// Otherwise, check if the current directory actually exists:
		cur_hash = ((*parent)->hash + djb2(cur_name)) % fs_table_size;
		cur_hash = linear_probe(cur_hash, cur_name, *parent, false);

		// If it doesn't exist:
		if (cur_hash == -1)
			// We can't go any further down the path.
			return NULL;

		// Otherwise keep going on:
		depth++;
		parent    = fs_table + cur_hash;
		cur_name  = next_name;
		next_name = strtok(NULL, "/");
	}

	// If the parent doesn't exist...
	if (   cur_name == NULL
	    || *parent  == FS_DELETED
	    || !(*parent)->is_dir
	    // ... or we are creating a new file exceeding the children limit or the maximum depth, or we are looking for a file when the parent has no children...
	    || !((new  && (*parent)->n_children < MAX_DIRECTORY_CHILDREN && depth < MAX_FILESYSTEM_DEPTH) || (!new && (*parent)->n_children > 0))
	)
		// Stop here, can't do anything.
		return NULL;

	// Otherwise look for the requested file (or the new cell where it has to be created):
	cur_hash = ((*parent)->hash + djb2(cur_name)) % fs_table_size;
	cur_hash = linear_probe(cur_hash, cur_name, *parent, new);

	// If the requested cell doesn't exist:
	if (cur_hash == -1)
		// Stop here, can't do anything.
		return NULL;

	if (new) {
		fs_table[cur_hash] = fs__new(cur_name, new_is_dir, *parent);
		fs_table_files++;
	}

	// And return the cell we found:
	return fs_table + cur_hash;
}

/**
 * Search all the files with the given name starting from cur and exploring the tree recursively.
 * @param cur : pointer to the file from which the search will start.
 * @param name: the name to search.
 * @param n   : reference to a counter where the number of matches will be stored.
 * @ret   an array of pointers to files which all have the same requested name.
 * @pre   cur is a valid file pointer (not NULL).
 */
fs_file_t** fs__all(fs_file_t* cur, const char* name, size_t* n) {
	fs_file_t **matches, **sub_matches, *child;
	size_t matches_size, sub_n;
	register size_t sub_i;

	matches      = NULL;
	matches_size = 0;
	*n           = 0;

	// If the current file's name matches:
	if (strcmp(cur->name, name) == 0) {
		// Add it to the matches:
		matches_size = 1;
		matches      = malloc_or_die(sizeof(fs_file_t*));
		matches[*n]  = cur;
		(*n)++;
	}

	// If the current file is a directory with at least one child:
	if (cur->is_dir && cur->n_children > 0) {
		child = cur->content.l_child;
		// While there still is a child to search into:
		while (child != NULL) {
			// Search the name in the child's subtree:
			sub_n       = 0;
			sub_matches = fs__all(child, name, &sub_n);

			// If there were matches in the child's subtree:
			if (sub_n > 0) {
				matches_size += sub_n;
				matches       = realloc_or_die(matches, sizeof(fs_file_t*) * matches_size);

				// Add them to the existing matches:
				for (sub_i = 0; sub_i < sub_n; sub_i++, (*n)++)
					matches[*n] = sub_matches[sub_i];
			}

			free(sub_matches);

			// Search next child:
			child = child->r_sibling;
		}
	}

	return matches;
}

/**
 * Trace the given file back until the root and return its full path.
 * @param cur: the file of which the path is requested.
 * @param len: the length of the path in characters, should be 0 (zero) for the first recursive call.
 * @ret   a string representing the full path to the file.
 * @pre   cur is a valid file pointer (not NULL).
 */
char* fs__uri(fs_file_t* cur, size_t len) {
	char* path;

	// If we reached the root:
	if (cur->parent == NULL) {
		// Allocate a string of the requested size and return it:
		path = malloc_or_die(sizeof(char) * (len + 1));
		path[0] = '\0';
		return path;
	}

	// Otherwise first go up:
	path = fs__uri(cur->parent, len + strlen(cur->name) + 1);
	// Then concatenate the current name with the partial path and return it:
	strcat(path, "/");
	strcat(path, cur->name);

	return path;
}

/**
 * Delete cur and all the files contained in its subtree exploring it recursively.
 * @param cur: address of a pointer to the file to delete.
 * @pre   cur must be the address of a ->content.l_child or ->r_sibling field of an existing file.
 * @post  the requested file and all its children have been deleted; their cells in the hash table contain the value FS_DELETED.
 * @pre   cur is the address of a valid file pointer (not NULL or referencing NULL).
 */
void fs__del(fs_file_t** cur) {
	fs_file_t* next;

	// If the current file is a directory:
	if ((*cur)->is_dir) {
		// First delete all its children:
		while ((*cur)->content.l_child != NULL) {
			fs__del(&(*cur)->content.l_child);
		}
	}

	// Then remove the current file from the table:
	fs_table[(*cur)->hash] = FS_DELETED;
	fs_table_files--;

	// Delete its content:
	(*cur)->parent->n_children--;
	if (!(*cur)->is_dir)
		free((*cur)->content.data);
	free((*cur)->name);

	// Fix the hole in the parent's children list:
	next = (*cur)->r_sibling;
	if (next != NULL)
		next->l_sibling = (*cur)->l_sibling;

	// Delete the current file and replace it's address with the one of its right sibling:
	free(*cur);
	*cur = next;
}

/**
 * Compare two paths using strcmp and return the result. Used as compare function for qsort.
 * @param a: first path to compare.
 * @param b: second path to compare.
 * @ret   <>= 0 wether a comes before b, after b or is equal to b.
 */
int fs__cmp(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}

/****************************************************
 *               FILESYSTEM FUNCTIONS               *
 ****************************************************/

/**
 * Create a file represented by the given path.
 * @param path  : the path representing the file to be created.
 * @param is_dir: whether the file to be created is a directory or not.
 * @post  in case of success the new file is in the proper position in both the hash table and the tree.
 * @out   RESULT_SUCCESS in case of success; RESULT_ERROR in case of error.
 */
void fs_create(char* path, bool is_dir) {
	fs_file_t** new_file;

	if (*path) {
		// Try to create the new file:
		new_file = fs__get(path, true, is_dir);

		// If the new file has been successfully created:
		if (new_file != NULL) {
			printf(RESULT_SUCCESS"\n");
			return;
		}
	}

	printf(RESULT_FAILURE"\n");
}

/**
 * Delete the file represented by the given path and, if requested and if any, all its children.
 * @param path     : the path representing the file to be deleted.
 * @param recursive: whether to delete all the file's children (recursively) or not.
 * @post  in case of success, the file has been removed from both the hash table and the tree.
 * @out   RESULT_SUCCESS in case of success; RESULT_ERROR in case of error.
 */
void fs_delete(char* path, bool recursive) {
	fs_file_t** victim;

	// Get the hash table cell of the victim:
	victim = fs__get(path, false, false);

	// If the cell actually contains a valid file:
	if (victim != NULL && *victim != FS_DELETED && *victim != NULL) {
		// And if either we have to delete recursively or the victim has no children:
		if (recursive || (*victim)->n_children == 0) {
			// Get the address of the previous tree node pointer and procede with the deletion:
			if ((*victim)->l_sibling != NULL)
				fs__del(&(*victim)->l_sibling->r_sibling);
			else
				fs__del(&(*victim)->parent->content.l_child);

			printf(RESULT_SUCCESS"\n");
			return;
		}
	}

	printf(RESULT_FAILURE"\n");
}

/**
 * Output the content of the file represented by the given path.
 * @param path: the path representing the file to read.
 * @out   RESULT_READ_SUCCESS followed by the file's content in case of success; RESULT_ERROR in case of error.
 */
void fs_read(char* path) {
	fs_file_t** file;

	// Get the hash table cell of the requested file:
	file = fs__get(path, false, false);

	// If the file actually exists and is not a directory, read its content:
	if (file != NULL && *file != NULL && !(*file)->is_dir) {
		printf(RESULT_READ_SUCCESS" %s\n", (*file)->content.data);
		return;
	}

	printf(RESULT_FAILURE"\n");
}

/**
 * Write the given data to the file represented by the given path.
 * @param path: the path representing the file to write to.
 * @param data: the string to be written to the file.
 * @post  the file contains the given data.
 * @out   RESULT_SUCCESS followed by the length of data in case of success; RESULT_ERROR in case of error.
 */
void fs_write(char* path, const char* data) {
	fs_file_t** file;
	size_t data_len;

	// Get the hash table cell of the requested file:
	file = fs__get(path, false, false);

	// If the file actually exists and is not a directory, write its content:
	if (file != NULL && *file != NULL && !(*file)->is_dir) {
		free((*file)->content.data);
		data_len = strlen(data);
		(*file)->content.data = malloc_or_die(data_len + 1);
		strcpy((*file)->content.data, data);

		printf(RESULT_SUCCESS" %zu\n", data_len);
		return;
	}

	printf(RESULT_FAILURE"\n");
}

/**
 * Find all the files of the filesystem with the given name.
 * @param name: the name to search for.
 * @out   RESULT_SUCCESS followed by the full path of the matching file, one line per match, sorted lexicographically, in case of success; RESULT_ERROR if no file with the given name is found.
 */
void fs_find(const char* name) {
	fs_file_t** found;
	register size_t i;
	char** paths;
	size_t n;

	// Get all the files that match the given name:
	found = fs__all(fs_root, name, &n);

	if (n > 0) {
		paths = malloc_or_die(sizeof(char*) * n);

		// Geat their full paths:
		for (i = 0; i < n; i++)
			paths[i] = fs__uri(found[i], 0);
		free(found);

		// Sort them:
		qsort(paths, n, sizeof(fs_file_t*), fs__cmp);

		// And print them out:
		for (i = 0; i < n; i++) {
			printf(RESULT_SUCCESS" %s\n", paths[i]);
			free(paths[i]);
		}

		free(paths);
		return;
	}

	printf(RESULT_FAILURE"\n");
}

/**
 * Destroy the whole filesystem tree (including root) and free all the space.
 * @post the whole filesystem tree and hashtable have been freed.
 */
void fs_exit(void) {
	while (fs_root->content.l_child != NULL)
		fs__del(&fs_root->content.l_child);

	free(fs_root);
	free(fs_table);
}
