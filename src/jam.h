
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

#define    internal static
#define      global static
#define thread_decl __declspec(thread)

typedef   signed long long    int        i64;
typedef unsigned long long    int        u64;
typedef unsigned              int        u32;
typedef                       int        i32;
typedef                       int        b32;
typedef                       int   temp_b32;
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

#define vec2(x,y) (vec2){(x),(y)}
#define vec2_add(a,b) ((vec2){(a).x+(b).x,(a).y+(b).y})
#define vec2_sub(a,b) ((vec2){(a).x-(b).x,(a).y-(b).y})
#define vec2_mul(a,b) ((vec2){(a).x*(b).x,(a).y*(b).y})
#define v2_dot(a,b) ((a).x*(b).x + (a).y*(b).y)


typedef union {
	struct{ i32 x, y; };
	i32 xy[2];
} vec2i;

typedef union {
	struct{ f32 x, y, z; };
	struct{ f32 r, g, b; };
	f32 xyz[3];
	f32 rgb[3];
} f32x3;

typedef union {
	struct{ f32 x, y, z, w; };
	struct{ f32 r, g, b, a; };
	struct{ f32 x0, y0, x1, y1; };
	f32 xyzw[4];
	f32 rgba[4];
	struct{
		vec2 xy;
		vec2 zw;
	};
} f32x4;

typedef union {
	struct{ u8 x, y, z, w; };
	struct{ u8 r, g, b, a; };
	u8  xyzw[4];
	u8  rgba[4];
	i32 x_i32;
} u8x4;

typedef u8x4 Color;

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
	vec2 basis_x,basis_y;
	vec2 center;
	vec2 translation;
} trans2d;

static inline trans2d trans2d_rotation(f32 rotation, vec2 center) {
	vec2 basis_x = (vec2){cos(rotation),sin(rotation)};
	vec2 basis_y = (vec2){-basis_x.y,basis_x.x};
	return (trans2d){basis_x,basis_y,center,vec2(0,0)};
}
static inline trans2d trans2d_translation(vec2 translation) {
	return (trans2d){vec2(1,0),vec2(0,1),vec2(0,0),translation};
}
//
// todo: simplify this...
//
// T + C + ((X - C) * BX + (Y - C) * BY)
//
// T + C +
//  X * BX - C * BX +
//  Y * BY - C * BY
//
static inline vec2 apply_trans2d(trans2d trans, vec2 v) {
	vec2 o =
	vec2_add(vec2_add(trans.translation,trans.center)
	, vec2_add(
		vec2_mul(trans.basis_x,vec2_sub(vec2(v.x,v.x),trans.center)),
		vec2_mul(trans.basis_y,vec2_sub(vec2(v.y,v.y),trans.center))));
	return o;
}



typedef struct Controller_State Controller_State;
struct Controller_State {
	b32 shoulders[2];
	b32 start;
	b32 back;
	b32 DPAD[4];
	b32 ABXY[4];
	b32 bthumbs[2];
	f32 triggers[2];
	vec2 thumbs[2];
};

enum {
	FORMAT_R_U8     = DXGI_FORMAT_R8_UNORM,
	FORMAT_RGBA_U8  = DXGI_FORMAT_R8G8B8A8_UNORM,
	FORMAT_RGBA_F32 = DXGI_FORMAT_R32G32B32A32_FLOAT,
};

enum {
	FILTER_POINT = 0,
	FILTER_LINEAR,
	FILTER_COUNT,
};

static char *j_obj2str[]={
	"none",
	"image",
	"sound",
	"texture",
};
typedef enum {
	JAM_NONE = 0,
	JAM_IMAGE,
	JAM_SOUND,
	JAM_TEXTURE,
} jam_type;

typedef struct jam_Object jam_Object;
struct jam_Object {
	elf_Object obj;
	i32 type;
};

typedef struct jam_Image jam_Image;
struct jam_Image {
	jam_Object base;
	vec2i size;
	// todo: could be allocated along with the object
	Color *pixels;
};

typedef struct jam_Texture jam_Texture;
struct jam_Texture {
	jam_Object base;
	vec2i size;
	ID3D11SamplerState *sampler;
	ID3D11ShaderResourceView *view;
	ID3D11Texture2D *texture;
};

typedef struct jam_Sound jam_Sound;
struct jam_Sound {
	jam_Object base;
	ma_sound sound;
};






typedef struct Per_Pass_Constants Per_Pass_Constants;
struct Per_Pass_Constants {
	f32x4 transform[4];
};
STATIC_ASSERT(!(sizeof(Per_Pass_Constants) & 15) && sizeof(Per_Pass_Constants) >= 64 && sizeof(Per_Pass_Constants) <= D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);

typedef i32 Index2D;

#define INDEX_BUFFER_FORMAT DXGI_FORMAT_R32_UINT

typedef struct Vertex2D Vertex2D;
struct Vertex2D {
	vec2 position;
	vec2 texcoords;
	u8x4  color;
	u8    padding[12];
};

STATIC_ASSERT(!(sizeof(Vertex2D) & 15));

typedef struct jam_Audio jam_Audio;
struct jam_Audio {
	ma_engine engine;
	// all the sounds in here are never referenced by
	// anyone else, they are freed on demand
	jam_Sound *voices[16];
};

typedef struct jam_State jam_State;
struct jam_State {
	// HWND window;
	vec2i window_dimensions;
	// b32 resizing;
	// u8 was_input[256];
	// u8 now_input[256];
	// u32   key;
	// u32   character;
	// vec2i mouse_xy;
	// vec2i mouse_wheel;

	jam_Audio audio;

	jam_Texture default_font_texture;

	temp_b32 resizable;
	i64 begin_cycle_clock;
	f64 clocks_to_seconds;
	f64 target_seconds_to_sleep;
	f64 pending_seconds_to_sleep;

	ID3D11InfoQueue     *info_queue;
	ID3D11Device        *device;
	ID3D11DeviceContext *context;
	ID3D11Query *time_frame_start_query;
	ID3D11Query *time_frame_end_query;


	jam_Texture fallback_texture;

	vec2i window_render_target_resolution;
	IDXGISwapChain2 *window_present_mechanism;
	ID3D11Texture2D *window_render_target;
	ID3D11RenderTargetView *window_render_target_view;

	vec2i base_resolution;
	ID3D11Texture2D *base_render_target_texture;
	ID3D11RenderTargetView *base_render_target_view;
	ID3D11ShaderResourceView *base_render_target_shader_view;

	ID3D11RasterizerState *default_rasterizer;
	ID3D11BlendState *default_blender;
	ID3D11DepthStencilState *default_depth_stencil;
	ID3D11SamplerState *samplers[FILTER_COUNT];

	ID3D11Buffer *constant_buffer;
	ID3D11PixelShader  *pixel_shader;
	ID3D11VertexShader *vertex_shader;
	ID3D11InputLayout  *input_layout;
	ID3D11Buffer *vertex_buffer;
	i32 vertex_buffer_capacity;
	i32 vertex_buffer_write;

};


typedef enum OS_Cursor {
	OS_Cursor_Pointer,
	OS_Cursor_IBar,
	OS_Cursor_LeftRight,
	OS_Cursor_UpDown,
	OS_Cursor_DownRight,
	OS_Cursor_UpRight,
	OS_Cursor_UpDownLeftRight,
	OS_Cursor_HandPoint,
	OS_Cursor_Disabled,
	OS_Cursor_COUNT,
} OS_Cursor;

/* Note: mouse button order must be left, right and middle,
so that conveniently, left + 0 is left, left + 1 is right */
#define OS_KEYDEF(_) \
_(NONE)\
_(CLOSE)\
_(ESCAPE)\
_(F1)_(F2)_(F3)_(F4)_(F5)_(F6)_(F7)_(F8)_(F9)\
_(F10)_(F11)_(F12)_(F13)_(F14)_(F15)_(F16)\
_(F17)_(F18)_(F19)_(F20)_(F21)_(F22)_(F23)_(F24)\
_(SCROLL_V)_(SCROLL_H)\
_(MOUSE_LEFT)_(MOUSE_RIGHT)_(MOUSE_MIDDLE)\
_(UP)\
_(LEFT)\
_(DOWN)\
_(RIGHT)\
/* end */

typedef enum OS_Key {
#define OSKEY(NAME) INPUT_##NAME,
	OS_KEYDEF(OSKEY)
#undef OSKEY
	INPUT_COUNT,
} OS_Key;

/* Todo: remove this */
global
char *INPUT_Name[] = {
#define OSKEY(NAME) #NAME,
	OS_KEYDEF(OSKEY)
#undef OSKEY
};


