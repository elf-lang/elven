
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTANTS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define TAU 6.28318530717958647692


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VECTORS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union {
	struct{ i32 x, y; };
	i32 xy[2];
} vec2i;

typedef union {
	struct{ f32 x, y; };
	f32 xy[2];
	f32 v[2];
} vec2;

typedef union {
	struct{ f32 x, y, z; };
	f32 xyz[2];
	f32 v[2];
} vec3;

typedef union {
	struct{ f32 x, y, z, w; };
	struct{ f32 r, g, b, a; };
	struct{ f32 x0, y0, x1, y1; };
	struct{ f32 u0, v0, u1, v1; };
	f32 xyzw[4];
	f32 rgba[4];
	f32 v[4];
	struct{
		vec2 xy;
		vec2 zw;
	};
} vec4;

typedef union {
	struct {
		vec4 x, y, z, w;
	};
	vec4 rows[4];
} Matrix;

// Todo, remove from math, is this more domain specific
typedef vec4 UV_Coords;

typedef union {
	struct{ u8 x, y, z, w; };
	struct{ u8 r, g, b, a; };
	u8 xyzw[4];
	u8 rgba[4];
} vec4_u8;

typedef vec4_u8 Color_rgba_u8;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATIC DATA /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Mixing matrices, for color transformations when sampling from a texture */

/* produces rgba(r, g, b, a)
* leaves the sample color as is */
static const Matrix MIXER_RGBA = (Matrix) {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

/* produces rgba(r, r, r, r)
* takes the red channel and uses it for all other colors,
* this is useful for sampling from a single channel texture
* and outputting an rgba color */
static const Matrix MIXER_RRRR = (Matrix) {
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
};

/* produces rgba(a, a, a, a) */
static const Matrix MIXER_AAAA = (Matrix) {
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1,
	0, 0, 0, 1
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATIC DATA /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline f32 f32_max(f32 x, f32 y) { return x > y ? x : y; }
static inline i32 i32_max(i32 x, i32 y) { return x > y ? x : y; }

static inline f32 f32_trim(f32 v, f32 min, f32 max) { return v < min ? min : v > max ? max : v; }
static inline i32 i32_trim(i32 v, i32 min, i32 max) { return v < min ? min : v > max ? max : v; }

static inline f32 f32_mix(f32 min, f32 max, f32 t) { return min + (max - min) * t; }
static inline f32 f32_unmix(f32 t, f32 src_min, f32 src_max) { return (t - src_min) / (src_max - src_min); }
static inline f32 f32_remix(f32 t, f32 src_min, f32 src_max, f32 dst_min, f32 dst_max) { return dst_min + (dst_max-dst_min) * ((t-src_min) / (src_max-src_min)); }


PORTABLEFUNC INLINEFUNC
vec2 v2(f32 x, f32 y) {
	return (vec2) { x, y };
}

PORTABLEFUNC INLINEFUNC
vec2 v2_from_v2i(vec2i v) {
	return v2((f32) v.x, (f32) v.y);
}

PORTABLEFUNC INLINEFUNC
vec4 v4(f32 x, f32 y, f32 z, f32 w) {
	return (vec4) { x, y, z, w };
}


PORTABLEFUNC INLINEFUNC
vec2i v2i(i32 x, i32 y) {
	return (vec2i) { x, y };
}




#define xy_do(o, a, b) { (a).x o (b).x, (a).y o (b).y }
#define xy_do_sub(a, b) xy_do(-, a, b)
#define xy_do_add(a, b) xy_do(+, a, b)
#define xy_do_mul(a, b) xy_do(*, a, b)
#define xy_do_div(a, b) xy_do(/, a, b)

#define v2_sub(a, b) ((vec2) xy_do_sub(a, b))
#define v2_add(a, b) ((vec2) xy_do_add(a, b))
#define v2_mul(a, b) ((vec2) xy_do_mul(a, b))

#define v2_mul1(a, b) ((vec2) xy_do_mul(a, v2(b, b)))

#define v2_div(a, b) ((vec2) xy_do_div(a, b))
#define v2_mul_add(a, b, c) v2_add(v2_mul(a, b), c)
#define v2_div_sub(a, b, c) v2_sub(v2_div(a, b), c)
#define v2_cross(a, b) ((a).x * (b).y - (a).y * (b).x)

PORTABLEFUNC
vec2 v2_mix(vec2 a, vec2 b, f32 t) {
	return v2(f32_mix(a.x, b.x, t), f32_mix(a.y, b.y, t));
}

PORTABLEFUNC INLINEFUNC
vec2 v2_pow(vec2 a, vec2 b) {
	return v2(powf(a.x, b.x), powf(a.y, b.y));
}

PORTABLEFUNC INLINEFUNC
vec2 v2_abs(vec2 a) {
	return v2(fabsf(a.x), fabsf(a.y));
}

#define SIGN(x) ((x) < 0 ? -1 : (x) > 0 ? +1 : 0)
PORTABLEFUNC INLINEFUNC
vec2 v2_sign(vec2 a) {
	return v2(SIGN(a.x), SIGN(a.y));
}

PORTABLEFUNC INLINEFUNC
f32 v2_dot(vec2 a, vec2 b) {
	return a.x * b.x + a.y * b.y;
}

PORTABLEFUNC
f32 v2_dot2(vec2 a) {
	return v2_dot(a, a);
}

PORTABLEFUNC
f32 v2_len(vec2 a) {
	return sqrtf(v2_dot2(a));
}

#define v2i_sub(a, b) ((vec2i) xy_do_sub(a, b))
#define v2i_add(a, b) ((vec2i) xy_do_add(a, b))
#define v2i_mul(a, b) ((vec2i) xy_do_mul(a, b))
#define v2i_div(a, b) ((vec2i) xy_do_div(a, b))





PORTABLEFUNC
Matrix matrix_from_scale_and_translation_xy(vec2 scale, vec2 translation) {
#if 1
	Matrix result = {
		scale.x,       0, 0, translation.x,
		0      , scale.y, 0, translation.y,
		0      ,       0, 1,             0,
		0      ,       0, 0,             1,
	};
#else
	Matrix result = {
		scale.x      ,             0, 0, 0,
		0            ,       scale.y, 0, 0,
		0            ,             0, 1, 0,
		translation.x, translation.y, 0, 1,
	};
#endif
	return result;
}

// -> -1 ... +1
PORTABLEFUNC
Matrix matrix_for_ndc_transform(vec2 size) {
	vec2 scale = v2_div(v2(2.0f, 2.0f), size);
	vec2 translation = v2(-1.0f, -1.0f);
	return matrix_from_scale_and_translation_xy(scale, translation);
}

PORTABLEFUNC INLINEFUNC
f32 vec4_dot(vec4 a, vec4 b) {
	f32 r = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	return r;
}

PORTABLEFUNC
vec4 matrix_mul_vec4(Matrix a, vec4 b) {
	vec4 r;
	r.x = vec4_dot(a.x, b);
	r.y = vec4_dot(a.y, b);
	r.z = vec4_dot(a.z, b);
	r.w = vec4_dot(a.w, b);
	return r;
}

PORTABLEFUNC
vec4 mul_matrix_v4(Matrix m, vec4 v) {
	vec4 c;
	for (int i = 0; i < 4; i ++) {
		c.xyzw[i] =
		v.xyzw[0] * m.rows[i].xyzw[0] +
		v.xyzw[1] * m.rows[i].xyzw[1] +
		v.xyzw[2] * m.rows[i].xyzw[2] +
		v.xyzw[3] * m.rows[i].xyzw[3];
	}
	return c;
}


