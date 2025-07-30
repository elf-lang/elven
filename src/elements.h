//
// common definitions used everywhere
//

#if !defined(TAU)
#define TAU 6.283185307179586
#endif


// printf
#include <stdio.h>


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

#define    internal static
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

#if !defined(true)
#define true ((i32) 1)
#endif

#if !defined(false)
#define false ((i32) 0)
#endif

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
	f32 xyzw[4];
	f32 rgba[4];
	struct{
		vec2 xy;
		vec2 zw;
	};
} vec4;

typedef union {
	struct{ u8 x, y, z, w; };
	struct{ u8 r, g, b, a; };
	u8  xyzw[4];
	u8  rgba[4];
	i32 x_i32;
} vec4_u8;

typedef vec4_u8 Color;

typedef struct {
	union{
		vec2i xy;
		struct{i32 x,y;};
	};
	union{
		struct{i32 size_x,size_y;};
		struct{vec2i size;};
		struct{i32 w,h;};
	};
} r_i32;

typedef struct {
	f32 x, y, w, h;
} r_f32;

#define vec2(x,y) (vec2){(x),(y)}
#define vec2_add(a,b) ((vec2){(a).x+(b).x,(a).y+(b).y})
#define vec2_sub(a,b) ((vec2){(a).x-(b).x,(a).y-(b).y})
#define vec2_mul(a,b) ((vec2){(a).x*(b).x,(a).y*(b).y})
#define v2_dot(a,b) ((a).x*(b).x + (a).y*(b).y)


enum {
	DOWN_BIT       = 1,
	PRESSED_BIT    = 2,
	RELEASED_BIT   = 4,
};

enum {
	BUTTON_NONE = 0,

	BUTTON_MOUSE_LEFT,
	BUTTON_MOUSE_MIDDLE,
	BUTTON_MOUSE_RIGHT,
	BUTTON_MOUSE_COUNT,

	BUTTON_MENU,
	BUTTON_BACK,
	BUTTON_DEBUG,

	BUTTON_LEFT,
	BUTTON_UP,
	BUTTON_RIGHT,
	BUTTON_DOWN,

	BUTTON_DPAD_LEFT,
	BUTTON_DPAD_UP,
	BUTTON_DPAD_RIGHT,
	BUTTON_DPAD_DOWN,

	BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4,
	BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9,

	BUTTON_COUNT = 256,
};

typedef struct {
	u8 u;
} Button;


#if !defined(ASSERT_ALWAYS)
#define ASSERT_ALWAYS(xx) \
do { \
	if (!(xx)){ \
		printf("%s[%i] %s(): '%s' triggered assertion\n",__FILE__,__LINE__,__func__,#xx); \
		__debugbreak(); \
	}; \
} while(0)
#endif


#if !defined(ASSERT)
	#if defined(_DEBUG)
		#define ASSERT(xx) ASSERT_ALWAYS(xx)
	#else
		#define ASSERT(xx)
	#endif
#endif


#define BLACK (Color) { 0  ,   0,   0,   0 }
#define WHITE (Color) { 255, 255, 255, 255 }

