// rendering api, because we like to hide things from other things...


#define FORMAT_SIZE(FORM) ((FORM) & 255)

typedef enum {
	R_USAGE_PATTERN_RARE_UPDATES = 0,
} R_UsagePattern;

// Note, low byte encodes the format size
typedef enum {
	FORMAT_NONE        =               0,
	R_FORMAT_R_U8      = sizeof(u8)  * 1,
	R_FORMAT_RGBA_U8   = sizeof(u8)  * 4,
	R_FORMAT_RGBA_F32  = sizeof(f32) * 4,
} R_Format;

#define R_GetFormatSize(x) ((x) & 255)

typedef enum {
	R_SAMPLER_NONE = 0,
	R_SAMPLER_POINT,
	R_SAMPLER_LINEAR,
	R_SAMPLER_COUNT_,
} R_Sampler;

#define BLEND_DISABLE R_BLENDER_NONE

typedef enum {
	R_BLENDER_NONE = 0,
	R_BLENDER_ALPHA_BLEND,
	R_BLENDER_COUNT_,
} R_Blender;

typedef enum {
	SHADER_NONE = 0,
	R_SHADER_SIMPLE,
	R_SHADER_FS_BLIT,
	SHADER_GRAYSCALE,
	SHADER_PALETTE,
	R_SHADER_COUNT_,
} R_Shader;

typedef enum {
	MODE_NONE = 0,
	R_TOPO_TRIANGLES,
	R_TOPO_LINES,
} R_Topology;

typedef vec4_u8 R_Color;

typedef struct {
	vec3          position;
	vec2         texcoords;
	R_Color          color;
	vec2     texture_blend;
} R_Vertex;

STATIC_ASSERT(!(sizeof(R_Vertex) & 15));
STATIC_ASSERT(sizeof(R_Vertex) == 32);

typedef struct UV_Map UV_Map;
struct UV_Map {
	u32          ncoords;
	UV_Coords     coords[];
};

typedef struct R_Texture R_Texture;
struct R_Texture {
	vec2i            reso;
	R_Format       format;
	R_Sampler      filter;
	R_Shader       shader;
	Matrix          mixer;
	Image_r_u8      image;
	b32             dirty;
	UV_Map        *uv_map;
	const char     *label;
};

// typedef u64 RID;
typedef R_Texture *RID;


#define RID_NONE ((RID)(0))


typedef struct {
	// Todo, there's no need to encode this for each pass, it's pretty hefty ...
	// We can use delta / instruction based encoding to encode only the fields that change ...
	rect_i32    viewport;
	rect_i32     scissor;
	RID           output;
	RID          texture;
	R_Sampler    sampler;
	R_Topology  topology;
	R_Shader      shader;
	R_Blender    blender;
	// Matrix     transform;
	Matrix         mixer;
} R_PassParams;


typedef struct {
	R_PassParams params;
	u64          offset;
	u64          length;
} R_Pass;


typedef struct {
	R_Pass       *passes;
	u32       num_passes;
	R_Vertex   *verts;
	u32         verts_size_in_bytes;
} R_DrawParams;


typedef struct {
	vec2i reso;
	RID offscreen_target;
} R_Window;



typedef struct R_Renderer R_Renderer;
R_Renderer *R_InitRenderer();


typedef enum {
	R_BIND_INPUT  = 1,
	R_BIND_OUTPUT = 2,
	R_DYNAMIC     = 4,
} R_CreationFlags;


typedef struct {
	R_CreationFlags      flags;
	R_Format             format;
	vec2i                reso;
	u32                  stride;
	void                *data;
	const char          *label;
} R_CreateTextureParams;

RID R_CreateTexturePro(R_Renderer *rend, R_CreateTextureParams params);
void R_ReleaseTexture(R_Renderer *rend, R_Texture *texture);
RID R_CreateTextureEx(R_Renderer *rend, R_CreationFlags flags, R_Format format, vec2i reso, u32 stride, void *data);
RID R_CreateTexture(R_Renderer *rend, R_Format format, vec2i reso, u32 stride, void *data);
RID R_CreateInputOutputTexture(R_Renderer *rend, R_Format format, vec2i reso);
RID R_CreateDuplexTexture(R_Renderer *rend, R_Format format, vec2i reso);


void R_UpdateTexture(R_Renderer *rend, RID texture, rect_i32 region, u32 stride, void *data);

// call after end frame
// void R_ReadWindowOutputData(R_Renderer *rend, u8 *memory, int stride);

R_Window *R_CreateWindow(R_Renderer *rend, WID os_window);
void R_UpdateWindowTargets(R_Renderer *rend, R_Window *window, vec2i reso);
RID R_GetWindowOutput(R_Window *window);
void R_PresentWindow(R_Renderer *rend, R_Window *window);

void R_ClearOutput(R_Renderer *rend, RID output, R_Color color);
void R_Draw(R_Renderer *rend, R_DrawParams params);

PORTABLEFUNC
RID r_new_texture_r_u8_from_image(R_Renderer *rend, R_UsagePattern pattern, Image_r_u8 image) {
	RID texture = R_CreateTexture(rend, R_FORMAT_R_U8, image.reso, image.elem_stride * sizeof(* image.data), image.data);
	texture->image = image;
	return texture;
}
