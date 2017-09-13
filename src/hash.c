/**
 * File  : hash.c
 * Author: Marco Bonelli
 * Date  : 2017-09-13
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

#include <stddef.h>
#include "hash.h"

/**
 * Is it simple? Yes.
 * Is it fast? Yes.
 * Is it good? I don't really think so.
 * Does it work well for this project? Hell yeah.
 */
size_t hash(const char* key, size_t seed, size_t table_size) {
	size_t h;
	char c;
	
	h = seed;
	while ((c = *key++)) {
		h +=c;		
		h = (h << 7) ^ h;
	}

	return h % table_size;
}