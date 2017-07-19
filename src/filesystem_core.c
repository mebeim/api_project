#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "filesystem_core.h"

/****************************************************
 *                      PRIVATE                     *
 ****************************************************/
 
static fs_file_t* const FS_DELETED        = (fs_file_t*) -1;
static float      const FS_TABLE_MAX_LOAD = 2.0 / 3.0;
static size_t     const FS_ROOT_HASH      = 0;
static char*      const FS_ROOT_NAME      = "#";

/**
 * Hash the string provided as key (currently using the Jenkins hash function, still open for better alternatives).
 * @param key: the string to be hashed.
 * @ret   the computed hash.
 */
static unsigned long hash(const char* key) {
	unsigned char* k;
	unsigned long h;

	h = 0;
	k = (unsigned char*)key;

    while (*k) {
        h += *k++;
        h += h << 10;
        h ^= h >> 6;
    }

    h += h << 3;
    h ^= h >> 11;
    h += h << 15;

    return h;
}

/**
 * Scan the table from a start index until a valid cell is found.
 * @param start : starting index.
 * @param key   : file name to match which was used as key to produce the initial hash.
 * @param parent: file parent to match.
 * @param new   : whether to search for a new (empty) cell or an existing file.
 * @ret   index of the wanted cell in the table, -1 if it doesn't exist.
 * @pre   start has been created as start = (parent->hash + hash(key)) % fs_table_size.
 */
static int linear_probe(size_t start, const char* key, const fs_file_t* parent, bool new) {
	register size_t h;

	h = start;

	if (new) {
		while (fs_table[h] != NULL && fs_table[h] != FS_DELETED) {
			if (fs_table[h]->parent == parent && strcmp(fs_table[h]->name, key) == 0)
				return -1;
			h = (h + 1) % fs_table_size;
		}
	} else {
		while (fs_table[h] != NULL && (fs_table[h] == FS_DELETED || fs_table[h]->parent != parent || strcmp(fs_table[h]->name, key) != 0))
			h = (h + 1) % fs_table_size;

		if (fs_table[h] == NULL)
			return -1;
	}

	return (int)h;
}

/**
 * Rehash the files starting from cur and exploring the tree recursively.
 * @param cur: pointer to the file to rehash.
 * @pre   cur is a valid file pointer (not NULL) which was already properly removed from the table.
 * @post  cur->hash is the new hash and new position in the table.
 */
static void rehash_all(fs_file_t* cur) {
	fs_file_t* child;

	if (cur->parent != NULL) {
		cur->hash = (cur->parent->hash + hash(cur->name)) % fs_table_size;
		cur->hash = linear_probe(cur->hash, cur->name, cur->parent, true);
		fs_table[cur->hash] = cur;
	}

	if (cur->is_dir) {
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
static void expand_table(void) {
	free(fs_table);
	fs_table_size *= 2;
	fs_table = malloc_null(fs_table_size, sizeof(fs_file_t*));
	rehash_all(fs_root);
}

/****************************************************
 *                      PUBLIC                      *
 ****************************************************/

inline void fs__init(void) {
	fs_table_files = 0;
	fs_table_size  = 1024 * 1024 / sizeof(fs_file_t*);
	fs_table       = malloc_null(fs_table_size, sizeof(fs_file_t*));
	fs_root        = fs__new(NULL, true, NULL);
}

inline void fs__exit(void) {
	while (fs_root->content.l_child != NULL)
		fs__del(&fs_root->content.l_child);

	free(fs_root);
	free(fs_table);
}

fs_file_t* fs__new(char* name, bool is_dir, fs_file_t* parent) {
	fs_file_t* new;

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

	if (parent == NULL) {
		new->hash      = FS_ROOT_HASH;
		new->name      = FS_ROOT_NAME;
		new->l_sibling = NULL;
		new->r_sibling = NULL;
	} else {
		new->hash      = (parent->hash + hash(name)) % fs_table_size;
		new->hash      = linear_probe(new->hash, name, parent, true);
		new->name      = malloc_or_die(strlen(name) + 1);
		strcpy(new->name, name);

		new->l_sibling = NULL;
		new->r_sibling = parent->content.l_child;
		if (new->r_sibling != NULL)
			new->r_sibling->l_sibling = new;

		parent->content.l_child = new;
		parent->n_children++;
	}

	return new;
}

fs_file_t** fs__get(char* path, bool new, bool new_is_dir) {
	fs_file_t* parent;
	register unsigned short depth;
	char *cur_name, *next_name;
	int cur_hash;

	depth     = 0;
	parent    = fs_root;
	cur_name  = strtok(path, "/");
	next_name = strtok(NULL, "/");
	cur_hash  = FS_ROOT_HASH;

	while (parent != NULL && next_name != NULL && depth < MAX_FILESYSTEM_DEPTH) {
		if (parent->n_children == 0)
			return NULL;

		cur_hash = (parent->hash + hash(cur_name)) % fs_table_size;
		cur_hash = linear_probe(cur_hash, cur_name, parent, false);

		if (cur_hash == -1)
			return NULL;

		depth++;
		parent    = fs_table[cur_hash];
		cur_name  = next_name;
		next_name = strtok(NULL, "/");
	}

	if (   cur_name == NULL
	    || parent  == NULL
	    || !parent->is_dir
	    || !((new  && parent->n_children < MAX_DIRECTORY_CHILDREN && depth < MAX_FILESYSTEM_DEPTH) || (!new && parent->n_children > 0))
	)
		return NULL;

	cur_hash = (parent->hash + hash(cur_name)) % fs_table_size;
	cur_hash = linear_probe(cur_hash, cur_name, parent, new);

	if (cur_hash == -1)
		return NULL;

	if (new) {
		fs_table[cur_hash] = fs__new(cur_name, new_is_dir, parent);
		fs_table_files++;
	}

	return fs_table + cur_hash;
}

fs_file_t** fs__all(fs_file_t* cur, const char* name, size_t* n) {
	fs_file_t **matches, **sub_matches, *child;
	size_t matches_size, sub_n;
	register size_t sub_i;

	matches      = NULL;
	matches_size = 0;
	*n           = 0;

	if (strcmp(cur->name, name) == 0) {
		matches_size = 1;
		matches      = malloc_or_die(sizeof(fs_file_t*));
		matches[*n]  = cur;
		(*n)++;
	}

	if (cur->is_dir && cur->n_children > 0) {
		child = cur->content.l_child;
		while (child != NULL) {
			sub_n       = 0;
			sub_matches = fs__all(child, name, &sub_n);

			if (sub_n > 0) {
				matches_size += sub_n;
				matches       = realloc_or_die(matches, sizeof(fs_file_t*) * matches_size);

				for (sub_i = 0; sub_i < sub_n; sub_i++, (*n)++)
					matches[*n] = sub_matches[sub_i];
			}

			free(sub_matches);
			child = child->r_sibling;
		}
	}

	return matches;
}

char* fs__uri(fs_file_t* cur, size_t len) {
	char* path;

	if (cur->parent == NULL) {
		path = malloc_or_die(sizeof(char) * (len + 1));
		path[0] = '\0';
		return path;
	}

	path = fs__uri(cur->parent, len + strlen(cur->name) + 1);
	strcat(path, "/");
	strcat(path, cur->name);

	return path;
}

void fs__del(fs_file_t** cur) {
	fs_file_t* next;

	if ((*cur)->is_dir) {
		while ((*cur)->content.l_child != NULL) {
			fs__del(&(*cur)->content.l_child);
		}
	}

	fs_table[(*cur)->hash] = FS_DELETED;
	fs_table_files--;

	(*cur)->parent->n_children--;
	if (!(*cur)->is_dir)
		free((*cur)->content.data);
	free((*cur)->name);

	next = (*cur)->r_sibling;
	if (next != NULL)
		next->l_sibling = (*cur)->l_sibling;

	free(*cur);
	*cur = next;
}

int fs__cmp(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}