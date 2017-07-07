#include <stdlib.h>
#include <string.h>
#include "utils.h"

void* malloc_or_die(const size_t sz) {
	void* mem;

	mem = malloc(sz);
	if (mem == NULL)
		exit(1);

	return mem;
}

void* calloc_or_die(size_t n, size_t sz) {
	void* mem;

	mem = calloc(n, sz);
	if (mem == NULL)
		exit(1);

	return mem;
}

void* realloc_or_die(void* mem, size_t sz) {
	mem = realloc(mem, sz);
	if (mem == NULL)
		exit(1);

	return mem;
}

void* malloc_null(size_t n, size_t sz) {
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
