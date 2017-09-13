/**
 * File  : hash.h
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

#ifndef API_PROJECT_HASH_INCLUDED
#define API_PROJECT_HASH_INCLUDED

/**
 * Hash the string provided as key for a table of the provided size and using the provided seed. Damn that's some good short description isn't it?
 * @param key       : the string to be hashed.
 * @param seed      : the seed (i.e. starting value).
 * @param table_size: size of the table to generate the hash for.
 * @ret   the computed hash.
 */
size_t hash(const char* key, size_t seed, size_t table_size);

#endif
