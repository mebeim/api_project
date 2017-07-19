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