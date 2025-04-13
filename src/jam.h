#if !defined(TAU)
#define TAU 6.283185307179586
#endif
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
} vec4;

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
	f32 x, y, w, h;
} r_f32;



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

	BUTTON_JUMP,
	BUTTON_DASH,
	BUTTON_USE,
	BUTTON_SHOW_MINIMAP,

	BUTTON_COUNT,
};

typedef struct {
	u8 u;
} Button;

// typedef struct {
// 	vec2 basis_x,basis_y;
// 	vec2 center;
// 	vec2 translation;
// } trans2d;

// static inline trans2d trans2d_rotation(f32 rotation, vec2 center) {
// 	vec2 basis_x = (vec2){cos(rotation),sin(rotation)};
// 	vec2 basis_y = (vec2){-basis_x.y,basis_x.x};
// 	return (trans2d){basis_x,basis_y,center,vec2(0,0)};
// }
// static inline trans2d trans2d_translation(vec2 translation) {
// 	return (trans2d){vec2(1,0),vec2(0,1),vec2(0,0),translation};
// }
//
// todo: simplify this...
//
// T + C + ((X - C) * BX + (Y - C) * BY)
//
// T + C +
//  X * BX - C * BX +
//  Y * BY - C * BY
//
// static inline vec2 apply_trans2d(trans2d trans, vec2 v) {
// 	vec2 o =
// 	vec2_add(vec2_add(trans.translation,trans.center)
// 	, vec2_add(
// 	vec2_mul(trans.basis_x,vec2_sub(vec2(v.x,v.x),trans.center)),
// 	vec2_mul(trans.basis_y,vec2_sub(vec2(v.y,v.y),trans.center))));
// 	return o;
// }

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

typedef enum {
	FORMAT_R_U8     = DXGI_FORMAT_R8_UNORM,
	FORMAT_RGBA_U8  = DXGI_FORMAT_R8G8B8A8_UNORM,
	FORMAT_RGBA_F32 = DXGI_FORMAT_R32G32B32A32_FLOAT,
} TextureType;


// static char *j_obj2str[]={
// 	"none",
// 	"image",
// 	"sound",
// 	"texture",
// };
// typedef enum {
// 	JAM_NONE = 0,
// 	JAM_IMAGE,
// 	JAM_SOUND,
// 	JAM_TEXTURE,
// } jam_type;

// typedef struct jam_Object jam_Object;
// struct jam_Object {
// 	elf_Object obj;
// 	i32 type;
// };

// typedef struct jam_Image jam_Image;
// struct jam_Image {
// 	jam_Object base;
// 	vec2i size;
// 	// todo: could be allocated along with the object
// 	Color *pixels;
// };

typedef struct rTextureStruct rTextureStruct;
struct rTextureStruct {
	vec2i resolution;
	ID3D11SamplerState       *sampler;
	ID3D11Texture2D          *texture;
	ID3D11ShaderResourceView *shader_resource_view;
	ID3D11RenderTargetView   *render_target_view;
};

// typedef struct jam_Sound jam_Sound;
// struct jam_Sound {
// 	jam_Object base;
// 	ma_sound sound;
// };



typedef struct Per_Pass_Constants Per_Pass_Constants;
struct Per_Pass_Constants {
	vec4 transform[4];
	vec4 storage[12];
};
STATIC_ASSERT(!(sizeof(Per_Pass_Constants) & 15) && sizeof(Per_Pass_Constants) >= 64 && sizeof(Per_Pass_Constants) <= D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);

typedef struct Vertex2D Vertex2D;
struct Vertex2D {
	vec2 position;
	vec2 texcoords;
	u8x4  color;
	u8    padding[12];
};

STATIC_ASSERT(!(sizeof(Vertex2D) & 15));

// typedef struct jam_Audio jam_Audio;
// struct jam_Audio {
// 	ma_engine engine;
// 	// all the sounds in here are never referenced by
// 	// anyone else, they are freed on demand
// 	jam_Sound *voices[16];
// };


typedef enum {
	MODE_NONE      = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
	MODE_TRIANGLES = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	MODE_LINES     = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
} Topology;

typedef enum {
	SAMPLER_NONE = 0,
	SAMPLER_POINT,
	SAMPLER_LINEAR,
	SAMPLER_CAPACITY,
} SamplerId;

typedef enum {
	BLENDER_NONE = 0,
	BLENDER_CAPACITY,
} BlenderId;

typedef enum {
	TEXTURE_NONE = 0,
	TEXTURE_DEFAULT,
	TEXTURE_FONT,
	TEXTURE_CAPACITY = 128,
} TextureId;

enum {
	SHADER_NONE = 0,
	SHADER_DEFAULT,
	SHADER_CAPACITY = 32,
};

typedef struct {
	i32 index;
} ShaderId;

typedef struct {
	Topology  topology;
	TextureId texture;
	ShaderId  shader;
	SamplerId sampler;
	BlenderId blender;
} Draw_State;

enum {
	FONT_NAME_CAPACITY = 128,
	FONT_GLYPHS_CAPACITY = 256,
};

typedef struct JFont JFont;
struct JFont {
	JFont *prox;
	TextureId texture;
	char name[FONT_NAME_CAPACITY];
	stbtt_bakedchar glyphs[FONT_GLYPHS_CAPACITY];
};

typedef struct JState JState;
struct JState {
	// MUST BE FIRST FIELD
	elf_State  R;
	elf_Module M;

	vec2i window_dimensions;

	Draw_State draw_prev;
	Draw_State draw_prox;
	JFont *font;

	temp_b32 resizable;
	i64 begin_cycle_clock;
	f64 clocks_to_seconds;
	f64 target_seconds_to_sleep;
	f64 pending_seconds_to_sleep;

	rTextureStruct textures[TEXTURE_CAPACITY];

	ID3D11InfoQueue     *info_queue;
	ID3D11Device        *device;
	ID3D11DeviceContext *context;
	ID3D11Query *time_frame_start_query;
	ID3D11Query *time_frame_end_query;
	vec2i window_render_target_resolution;
	IDXGISwapChain2 *window_present_mechanism;
	ID3D11Texture2D *window_render_target;
	ID3D11RenderTargetView *window_render_target_view;

	ID3D11PixelShader *shaders[SHADER_CAPACITY];

	vec2i base_resolution;
	ID3D11Texture2D *base_render_target_texture;
	ID3D11RenderTargetView *base_render_target_view;
	ID3D11ShaderResourceView *base_render_target_shader_view;
	ID3D11RasterizerState *default_rasterizer;
	ID3D11BlendState *default_blender;
	ID3D11DepthStencilState *default_depth_stencil;
	ID3D11Buffer *constant_buffer;
	ID3D11VertexShader *vertex_shader;
	ID3D11InputLayout  *input_layout;

	ID3D11SamplerState *samplers[SAMPLER_CAPACITY];

	ID3D11Buffer *vertices_submission_buffer;
	i32 vertices_submission_buffer_capacity;
	i32 vertices_submission_buffer_offset;
};