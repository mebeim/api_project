#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "filesystem_core.h"
#include "filesystem_api.h"

void fs_create(char* path, bool is_dir) {
	fs_file_t** new_file;
	fs_file_t* parent;
	char *p, *last_slash, *new_name;
	unsigned short n_slashes;
	int new_hash;

	p          = path;
	last_slash = NULL;
	n_slashes  = 0;

	while (*p && n_slashes <= MAX_FILESYSTEM_DEPTH) {
		if (*p == '/') {
			last_slash = p;
			n_slashes++;
		}

		p++;
	}

	if (n_slashes > 0 && n_slashes <= MAX_FILESYSTEM_DEPTH) {
		new_name = malloc_or_die(strlen(last_slash));
		new_name = strcpy(new_name, last_slash + 1);
		new_file = fs__get(path, &parent, true);

		if (new_file != NULL) {
			new_hash  = (parent->hash + djb2(new_name)) % fs_table_size;
			new_hash  = linear_probe(new_hash, new_name, parent, true);
			*new_file = fs__new(new_hash, new_name, is_dir, parent);
			fs_table_files++;

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

	victim = fs__get(path, NULL, false);

	if (victim != NULL && *victim != FS_DELETED && *victim != NULL) {
		if (recursive || (*victim)->n_children == 0) {
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

	file = fs__get(path, NULL, false);

	if (file != NULL && *file != NULL && !(*file)->is_dir) {
		printf(RESULT_READ_SUCCESS" %s\n", (*file)->content.data);
		return;
	}

	printf(RESULT_FAILURE"\n");
}

void fs_write(char* path, const char* data) {
	fs_file_t** file;
	size_t data_len;

	file = fs__get(path, NULL, false);

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

	found = fs__all(fs_root, name, &n);

	if (n > 0) {
		paths = malloc_or_die(sizeof(char*) * n);

		for (i = 0; i < n; i++)
			paths[i] = fs__uri(found[i], 0);
		free(found);

		qsort(paths, n, sizeof(fs_file_t*), fs__cmp);

		for (i = 0; i < n; i++) {
			printf(RESULT_SUCCESS" %s\n", paths[i]);
			free(paths[i]);
		}

		free(paths);
		return;
	}

	printf(RESULT_FAILURE"\n");
}
