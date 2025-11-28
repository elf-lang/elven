typedef u32 hash32;

hash32 hash_murmurhash1(u32 *chunks, u32 nchunks, u32 seed);
hash32 hash_text(const char *text, u32 size);
