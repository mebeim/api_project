/**
 * File  : filesystem_api.c
 * Author: Marco Bonelli
 * Date  : 2017-07-20
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

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "filesystem_core.h"
#include "filesystem_api.h"

void fs_init(void) {
	fs__init();
}

void fs_exit(void) {
	fs__exit();
}

void fs_create(char* path, bool is_dir) {
	fs_file_t** new_file;

	if (*path) {
		new_file = fs__get(path, true, is_dir);

		if (new_file != NULL) {
			printf(RESULT_SUCCESS"\n");
			return;
		}
	}

	printf(RESULT_FAILURE"\n");
}

void fs_delete(char* path, bool recursive) {
	fs_file_t** victim;

	victim = fs__get(path, false, false);

	if (victim != NULL && *victim != NULL) {
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

	file = fs__get(path, false, false);

	if (file != NULL && *file != NULL && !(*file)->is_dir) {
		printf(RESULT_READ_SUCCESS" %s\n", (*file)->content.data);
		return;
	}

	printf(RESULT_FAILURE"\n");
}

void fs_write(char* path, const char* data) {
	fs_file_t** file;
	size_t data_len;

	file = fs__get(path, false, false);

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