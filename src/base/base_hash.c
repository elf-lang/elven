

hash32 hash_murmurhash1(u32 *chunks, u32 nchunks, u32 seed) {
	enum { PRIME = 0xc6a4a793 };
	hash32 h = seed ^ (nchunks * PRIME);

	for (int i = 0; i < nchunks; ++ i) {
		h += chunks[i];
		h *= PRIME;
		h ^= h >> 16;
	}

	h *= PRIME;
	h ^= h >> 10;
	h *= PRIME;
	h ^= h >> 17;
	return h;
}

hash32 hash_text(const char *text, u32 size) {
	hash32 hash = 5381;
	for (int i = 0; i < size; ++ i) {
		hash = ((hash << 5) + hash) + text[i];
	}
	return hash;
}
