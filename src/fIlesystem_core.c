#include <stdbool.h>
#include <string.h>
#include "utils.h"

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

	// If a parent is provided (i.e. the file to be created is not the root):
	if (parent != NULL) {
		// Insert the new file in the head of the list of children of its parent:
		new->l_sibling = NULL;
		new->r_sibling = parent->content.l_child;
		if (new->r_sibling != NULL)
			new->r_sibling->l_sibling = new;

		parent->content.l_child = new;
		parent->n_children++;
	} else {
		// Otherwise we're creating the root, and it hasn't got siblings.
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

	// While the parent exists, and we're not at the last file name in the path:
	while (   parent    != NULL
	       && *parent   != NULL
	       && *parent   != FS_DELETED
	       && cur_name  != NULL
	       && next_name != NULL
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
		parent    = fs_table + cur_hash;
		cur_name  = next_name;
		next_name = strtok(NULL, "/");
	}

	// If the parent doesn't exist...
	if (   cur_name == NULL
		|| *parent  == FS_DELETED
		|| !(*parent)->is_dir
		// ... or we are creating a new file exceeding the children limit, or we are looking for a file when the parent has no children:
		|| !((new  && (*parent)->n_children < MAX_DIRECTORY_CHILDREN) || (!new && (*parent)->n_children > 0))
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

	// If the caller requested it:
	if (parent_arg != NULL)
		// Pass a pointer to the parent:
		*parent_arg = *parent;

	// And return the cell we found:
	return fs_table + cur_hash;
}

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

int fs__cmp(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}