//
// common stuff
//



#if !defined(TAU)
#define TAU 6.283185307179586
#endif


#define U8_MAX  ((u8)  ~0)
#define U16_MAX ((u16) ~0)
#define U32_MAX ((u32) ~0)
#define U64_MAX ((u64) ~0)




#include <stdio.h>
#include <assert.h>




#if !defined(TRACELOG)
#define TRACELOG(msg,...) printf(msg"\n",__VA_ARGS__)
#endif

#if !defined(MEGABYTES)
#define MEGABYTES(A) ((i64)(A) << 20LLU)
#endif

#if !defined(KILOBYTES)
#define KILOBYTES(A) ((i64)(A) << 10LLU)
#endif

#if !defined(MIN)
#define MIN(A,B) (((A) < (B)) ? (A) : (B))
#endif

#if !defined(MAX)
#define MAX(A,B) (((A) > (B)) ? (A) : (B))
#endif

#if !defined(ABS)
#define ABS(A) (((A) < 0) ? -(A) : (A))
#endif

#if !defined(COUNTOF)
#define COUNTOF(X) (sizeof(X) / sizeof((X)[0]))
#endif


#if !defined(STATIC_ASSERT)
#define STATIC_ASSERT(x) typedef char _static_assert_[x ? 1 : -1]
#endif


#if !defined(true)
#define true ((i32) 1)
#endif

#if !defined(false)
#define false ((i32) 0)
#endif


// #define    internal static
#define      global static
#define thread_decl __declspec(thread)


typedef   signed long long    int        i64;
typedef unsigned long long    int        u64;
typedef unsigned              int        u32;
typedef                       int        i32;
typedef                       int        b32;
typedef   signed            short        i16;
typedef unsigned            short        u16;
typedef unsigned             char         u8;
typedef   signed             char         i8;
typedef                     float        f32;
typedef                    double        f64;

typedef union {
	struct{ f32 x, y; };
	f32 xy[2];
} vec2;

typedef union {
	struct{ i32 x, y; };
	i32 xy[2];
} vec2i;

typedef union {
	struct{ f32 x, y, z; };
	struct{ f32 r, g, b; };
	f32 xyz[3];
	f32 rgb[3];
} vec3;

typedef union {
	struct{ f32 x, y, z, w; };
	struct{ f32 r, g, b, a; };
	f32  xyzw[4];
	f32  rgba[4];
	vec3 xyz;
	struct { vec2 xy, zw; };
} vec4;

typedef union {
	struct{ u8 x, y, z, w; };
	struct{ u8 r, g, b, a; };
	u8  xyzw[4];
	u8  rgba[4];
	u32 packed;
} vec4_u8;


typedef struct {
	vec4 rows[4];
} Matrix;


#define IDENTITY_MATRIX() ((Matrix) { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 })


typedef vec4_u8 Color;


// todo: proper struct order to avoid having to do this...
static inline u32 __color_pack(Color c) {
	return c.r << 24 | c.g << 16 | c.b << 8 | c.a << 0;
}

static inline Color __color_unpack(u32 p) {
	return (Color){ p >> 24, p >> 16, p >> 8, p >> 0 };
}


#define COLOR_PACK   __color_pack
#define COLOR_UNPACK	__color_unpack



typedef union {
	struct { vec2i xy, wh; };
	struct { i32 x, y, w, h; };
} iRect;

typedef union {
	struct { vec2 xy, wh; };
	struct { f32 x, y, w, h; };
} Rect;



typedef struct {
	vec2i reso;
	Color data[];
} Image;






#define DOWN_SHIFT     0
#define PRESSED_SHIFT  1
#define RELEASED_SHIFT 2
#define REPEAT_SHIFT   3
enum {
	BUTTON_DOWN     = 1 << DOWN_SHIFT,
	BUTTON_PRESSED  = 1 << PRESSED_SHIFT,
	BUTTON_RELEASED = 1 << RELEASED_SHIFT,
	BUTTON_REPEAT   = 1 << REPEAT_SHIFT,
};

typedef u8 Button;


static inline Button NewButton(Button b, int d) {
	int r_bit = d << REPEAT_SHIFT;
	int d_bit = d << DOWN_SHIFT;
	int p_bit = (b ^ d & 1) << (RELEASED_SHIFT - d);
	return p_bit | d_bit | r_bit;
}


#define ASSERT assert


#define BLACK (Color) { 0  ,   0,   0,   0 }
#define WHITE (Color) { 255, 255, 255, 255 }

#define vec2(x,y) (vec2){(x),(y)}
#define vec2_add(a,b) ((vec2){(a).x+(b).x,(a).y+(b).y})
#define vec2_sub(a,b) ((vec2){(a).x-(b).x,(a).y-(b).y})
#define vec2_mul(a,b) ((vec2){(a).x*(b).x,(a).y*(b).y})
#define v2_dot(a,b) ((a).x*(b).x + (a).y*(b).y)




static int unshift(int x) {
	// todo:!
	return __builtin_ffsll(x);
}


#define ISPOW2(x) ((x) != 0 && !((x) & ((x) - 1)))
#define LOG2MASK(x) ((1 << (x)) - 1)
