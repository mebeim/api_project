#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "filesystem_core.h"

void fs_create(char* path, bool is_dir) {
	fs_file_t** new_file;
	fs_file_t* parent;
	char *p, *last_slash, *new_name;
	unsigned short n_slashes;
	int new_hash;

	p          = path;
	last_slash = NULL;
	n_slashes  = 0;

	// Check the number of slashes and save the position of the last one to get the new file's name:
	while (*p && n_slashes <= MAX_FILESYSTEM_DEPTH) {
		if (*p == '/') {
			last_slash = p;
			n_slashes++;
		}

		p++;
	}

	if (n_slashes > 0 && n_slashes <= MAX_FILESYSTEM_DEPTH) {
		// Copy the new name beforehand, or strtok will mess with it:
		new_name = malloc_or_die(strlen(last_slash));
		new_name = strcpy(new_name, last_slash + 1);
		// Get the hash table cell which will hold the new file:
		new_file = fs__get(path, &parent, true);

		// If the position is valid:
		if (new_file != NULL) {
			// Calculate the new file's hash and create it:
			new_hash  = (parent->hash + djb2(new_name)) % fs_table_size;
			new_hash  = linear_probe(new_hash, new_name, parent, true);
			*new_file = fs__new(new_hash, new_name, is_dir, parent);
			fs_table_files++;

			// Expand the hash table if the maximum load coefficent has been exceeded:
			if (((float)fs_table_files / (float)fs_table_size) > FS_TABLE_MAX_LOAD)
				expand_table();

			printf(RESULT_SUCCESS"\n");
			return;
		}

		free(new_name);
	}

	printf(RESULT_FAILURE"\n");
}

void fs_delete(char* path, bool recursive) {
	fs_file_t** victim;

	// Get the hash table cell of the victim:
	victim = fs__get(path, NULL, false);

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

void fs_read(char* path) {
	fs_file_t** file;

	// Get the hash table cell of the requested file:
	file = fs__get(path, NULL, false);

	// If the file actually exists and is not a directory, read its content:
	if (file != NULL && *file != NULL && !(*file)->is_dir) {
		printf(RESULT_READ_SUCCESS" %s\n", (*file)->content.data);
		return;
	}

	printf(RESULT_FAILURE"\n");
}

void fs_write(char* path, const char* data) {
	fs_file_t** file;
	size_t data_len;

	// Get the hash table cell of the requested file:
	file = fs__get(path, NULL, false);

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