/**
 * File  : main.c
 * Author: Marco Bonelli
 * Date  : 2017-07-27
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

	fs_init();
	done = false;

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
				str = strtok(NULL, "\r\n");
				if (str != NULL) {
					str = strchr(str, '"');
					str = strtok(str, "\"");
				}

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
