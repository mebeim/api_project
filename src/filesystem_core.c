#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "filesystem_core.h"

/***********************
 **      PRIVATE      **
 ***********************/

/**
 * Rehash the files starting from cur and exploring the tree recursively.
 * @param cur: pointer to the file to rehash.
 * @pre   cur is a valid file pointer (not NULL) which was already properly removed from the table.
 * @post  cur->hash is the new hash and new position in the table.
 */
static void rehash_all(fs_file_t*);

/**********************
 **      PUBLIC      **
 **********************/

fs_file_t* fs__new(size_t hash, char* name, bool is_dir, fs_file_t* parent) {
	fs_file_t* new;

	new             = malloc_or_die(sizeof(fs_file_t));
	new->hash       = hash;
	new->name       = name;
	new->is_dir     = is_dir;
	new->n_children = 0;
	new->parent     = parent;

	if (is_dir)
		new->content.l_child = NULL;
	else
		new->content.data = calloc_or_die(1, sizeof(char));

	if (parent != NULL) {
		new->l_sibling = NULL;
		new->r_sibling = parent->content.l_child;
		if (new->r_sibling != NULL)
			new->r_sibling->l_sibling = new;

		parent->content.l_child = new;
		parent->n_children++;
	} else {
		new->l_sibling = NULL;
		new->r_sibling = NULL;
	}

	return new;
}

fs_file_t** fs__get(char* path, fs_file_t** parent_arg, bool new) {
	fs_file_t** parent;
	char *cur_name, *next_name;
	int cur_hash;

	parent    = &fs_root;
	cur_name  = strtok(path, "/");
	next_name = strtok(NULL, "/");
	cur_hash  = FS_ROOT_HASH;

	while (   parent    != NULL
	       && *parent   != NULL
	       && *parent   != FS_DELETED
	       && cur_name  != NULL
	       && next_name != NULL
	) {
		if (!((*parent)->n_children > 0))
			return NULL;

		cur_hash = ((*parent)->hash + djb2(cur_name)) % fs_table_size;
		cur_hash = linear_probe(cur_hash, cur_name, *parent, false);

		if (cur_hash == -1)
			return NULL;

		parent    = fs_table + cur_hash;
		cur_name  = next_name;
		next_name = strtok(NULL, "/");
	}

	if (   cur_name == NULL
		|| *parent  == FS_DELETED
		|| !(*parent)->is_dir
		|| !((new  && (*parent)->n_children < MAX_DIRECTORY_CHILDREN) || (!new && (*parent)->n_children > 0))
	)
		return NULL;

	cur_hash = ((*parent)->hash + djb2(cur_name)) % fs_table_size;
	cur_hash = linear_probe(cur_hash, cur_name, *parent, new);

	if (cur_hash == -1)
		return NULL;

	if (parent_arg != NULL)
		*parent_arg = *parent;

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
