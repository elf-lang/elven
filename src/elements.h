//
// common stuff
//


#include <stdio.h>
#include <assert.h>

typedef vec4_u8 Color;

#define IDENTITY_MATRIX() ((Matrix) { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 })


// todo: proper struct order to avoid having to do this...
static inline u32 __color_pack(vec4_u8 c) {
	return c.r << 24 | c.g << 16 | c.b << 8 | c.a << 0;
}

static inline vec4_u8 __color_unpack(u32 p) {
	return (vec4_u8){ p >> 24, p >> 16, p >> 8, p >> 0 };
}


#define COLOR_PACK   __color_pack
#define COLOR_UNPACK	__color_unpack


// typedef union {
// 	struct { vec2 xy, wh; };
// 	struct { f32 x, y, w, h; };
// } rect_i32;

typedef struct {
	vec2i reso;
	vec4_u8 data[];
} Image;



#define ASSERT assert

static int unshift(int x) {
	// todo:!
	return __builtin_ffsll(x);
}


#define LOG2MASK(x) ((1 << (x)) - 1)

