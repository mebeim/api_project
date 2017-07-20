/**
 * File  : hash.h
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

#ifndef API_PROJECT_HASH_INCLUDED
#define API_PROJECT_HASH_INCLUDED

/**
 * Hash the string provided as key using the provided seed. C implementation of MetroHash64 adapted from https://github.com/rurban/smhasher.
 * @param key:  the string to be hashed.
 * @param seed: the seed (wow who would've guessed that).
 * @ret   the computed hash.
 */
unsigned long hash(const char* key, const unsigned long seed);

#endif