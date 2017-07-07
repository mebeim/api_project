#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "filesystem_core.h"
#include "filesystem_api.h"

#define COMMAND_CREATE 'c'
#define COMMAND_DELETE 'd'
#define COMMAND_READ   'r'
#define COMMAND_WRITE  'w'
#define COMMAND_FIND   'f'
#define COMMAND_EXIT   'e'

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