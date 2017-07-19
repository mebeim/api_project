#include <stdint.h>
#include <string.h>
#include "hash.h"

/****************************************************
 *						PRIVATE						*
 ****************************************************/

static inline uint64_t crotate_r(uint64_t v, unsigned k) {
	return (v >> k) | (v << (64 - k));
}

static inline uint64_t cread_u64(const void* const ptr) {
	return *(uint64_t*)ptr;
}

static inline uint64_t cread_u32(const void* const ptr) {
	return *(uint32_t*)ptr;
}

static inline uint64_t cread_u16(const void* const ptr) {
	return *(uint16_t*)ptr;
}

static inline uint64_t cread_u8 (const void* const ptr) {
	return *(uint8_t*)ptr;
}

/****************************************************
 *						PUBLIC						*
 ****************************************************/

unsigned long hash(const char* key, const unsigned long seed) {
	const uint8_t *ukey, *ptr;
	uint64_t h, len, v[4], v0, v1;
	static const uint64_t k0 = 0xC83A91E1;
	static const uint64_t k1 = 0x8648DBDB;
	static const uint64_t k2 = 0x7BDEC03B;
	static const uint64_t k3 = 0x2F5870A5;

	ukey = (const uint8_t*)key;
	ptr	 = ukey;
	len	 = strlen(key);
	h    = ((((uint64_t)seed) + k2) * k0) + len;

	const uint8_t* const end = ptr + len;

	if (len >= 32) {
		v[0] = h;
		v[1] = h;
		v[2] = h;
		v[3] = h;

		do {
			v[0] += cread_u64(ptr) * k0; ptr += 8; v[0] = crotate_r(v[0],29) + v[2];
			v[1] += cread_u64(ptr) * k1; ptr += 8; v[1] = crotate_r(v[1],29) + v[3];
			v[2] += cread_u64(ptr) * k2; ptr += 8; v[2] = crotate_r(v[2],29) + v[0];
			v[3] += cread_u64(ptr) * k3; ptr += 8; v[3] = crotate_r(v[3],29) + v[1];
		} while (ptr <= (end - 32));

		v[2] ^= crotate_r(((v[0] + v[3]) * k0) + v[1], 33) * k1;
		v[3] ^= crotate_r(((v[1] + v[2]) * k1) + v[0], 33) * k0;
		v[0] ^= crotate_r(((v[0] + v[2]) * k0) + v[3], 33) * k1;
		v[1] ^= crotate_r(((v[1] + v[3]) * k1) + v[2], 33) * k0;
		h += v[0] ^ v[1];
	}

	if ((end - ptr) >= 16) {
		v0	= h + (cread_u64(ptr) * k0); ptr += 8; v0 = crotate_r(v0,33) * k1;
		v1	= h + (cread_u64(ptr) * k1); ptr += 8; v1 = crotate_r(v1,33) * k2;
		v0 ^= crotate_r(v0 * k0, 35) + v1;
		v1 ^= crotate_r(v1 * k3, 35) + v0;
		h  += v1;
	}

	if ((end - ptr) >= 8) {
		h += cread_u64(ptr) * k3; ptr += 8;
		h ^= crotate_r(h, 33) * k1;

	}

	if ((end - ptr) >= 4) {
		h += cread_u32(ptr) * k3; ptr += 4;
		h ^= crotate_r(h, 15) * k1;
	}

	if ((end - ptr) >= 2) {
		h += cread_u16(ptr) * k3; ptr += 2;
		h ^= crotate_r(h, 13) * k1;
	}

	if ((end - ptr) >= 1) {
		h += cread_u8 (ptr) * k3;
		h ^= crotate_r(h, 25) * k1;
	}

	h ^= crotate_r(h, 33);
	h *= k0;
	h ^= crotate_r(h, 33);

	return (unsigned long)h;
}