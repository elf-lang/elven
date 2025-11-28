
// Todo, take in a stack for all these ops...
PORTABLEFUNC
char *format_size_decomposed_complete(u64 size) {
	u64 gb = size >> 30;
	u64 mb = size >> 20 & KB_MASK;
	u64 kb = size >> 10 & KB_MASK;
	u64  b = size >>  0 & KB_MASK;
	char *result = temp_format_("(size %lli: %lli GBs + %lli MBs + %lli KBs + %lli bytes)", size, gb, mb, kb, b);
	return result;
}

// Todo, will the compiler figure this out or do I have to use the builtin bit counter
PORTABLEFUNC
u32 bf_unshift(u64 v) {
	u32 r;
	for (r=0; ~v & 1; r ++, v >>= 1);
	return r;
}

typedef struct {
	i16 sign;
	i16 exponent;
	u64 mantissa;
} f64_decoded;


PORTABLEFUNC INLINEFUNC
f64_decoded f64_decode(f64 v) {
	union { f64 f; u64 u; } p = { v };
	f64_decoded dec;
	dec.sign     = p.u >> 63 & 0x1;
	dec.exponent = p.u >> 52 & 0x7FF;
	dec.mantissa = p.u >>  0 & 0xFFFFFFFFFFFFFULL;
	return dec;
}

PORTABLEFUNC
char *format_u64_base_log2(char *e, u32 base, u64 numr) {
	char *dgts = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	u32 tail = 64;
	u32 mask = (1 << base) - 1;
	do * -- e = dgts[numr & mask], numr >>= base; while (numr > 0);
	return e;
}

PORTABLEFUNC
char *format_u64(char *e, u32 base, u32 padd, u64 numr) {
	char *d = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (base == 0 || base > 36) {
		goto esc;
	}
	u32 mask = base - 1;
	if (!(base & mask)) {
		u32 shft = bf_unshift(base);
		do * -- e = d[numr & mask], numr >>= shft, -- padd; while (numr > 0);
	}
	else {
		do * -- e = d[numr % base], numr  /= base, -- padd; while (numr > 0);
	}
	while (padd > 0) * -- e = '0', -- padd;
	esc:
	return e;
}

PORTABLEFUNC
char *format_f64(char *e, u32 base, u32 padd, u32 prec, f64 v) {
	// ensure positive
	f64 k = ABS(v);
	// extract integer part
	u64 x = (u64) k;
	// collect fractional part
	f64 f = k;
	u64 p = x;
	u64 y = 0;
	for (i32 i = 0; i < prec; ++ i) {
		f = (f - (f64) p) * base;
		p = (u64) f;
		y *= base, y += p;
	}
	// round up
	y += (f - p) >= 0.5f;

	// write fractional part
	e = format_u64(e, base, prec, y);
	// '.'
	* -- e = '.';
	// write integer part
	e = format_u64(e, base, padd, x);
	// sign is handled by caller
	return e;
}