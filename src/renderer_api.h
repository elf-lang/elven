// rendering api, because we like to hide things from other things...


#define FORMAT(ID, TYPE, CHAN) (((ID) << 8) | (sizeof((TYPE)) * (CHAN)))
#define FORMAT_SIZE(FORM) ((FORM) & 255)

typedef enum {
	FORMAT_NONE      = 0,
	FORMAT_R_U8      = 1,
	FORMAT_RGBA_U8   = 2,
	FORMAT_RGBA_F32  = 3,
} TextureFormat;

#define GetTextureFormatSize(x) (g_format_to_size[(x)])
static const i32 g_format_to_size[] = {
	[FORMAT_NONE]       =  0,
	[FORMAT_R_U8]       =  1,
	[FORMAT_RGBA_U8]    =  4,
	[FORMAT_RGBA_F32]   = 16,
};




typedef enum {
	SAMPLER_NONE = 0,
	SAMPLER_POINT,
	SAMPLER_LINEAR,
	SAMPLER_CAPACITY,
} SamplerId;


#define BLEND_DISABLE BLENDER_NONE

typedef enum {
	BLENDER_NONE = 0,
	BLENDER_ALPHA_BLEND,
	BLENDER_CAPACITY,
} BlenderId;


typedef enum {
	SHADER_NONE = 0,
	SHADER_DEFAULT,
	SHADER_GRAYSCALE,
	SHADER_PALETTE,
	SHADER_CAPACITY,
} ShaderId;



typedef enum {
	MODE_NONE     ,
	TOPO_TRIANGLES,
	MODE_LINES    ,
} Topology;


typedef struct {
	vec3    position;
	vec2   texcoords;
	Color      color;
	u8          _[8];
} R_Vertex3;

STATIC_ASSERT(!(sizeof(R_Vertex3) & 15));
STATIC_ASSERT(sizeof(R_Vertex3) == 32);


typedef struct {
	f32 u0, v0, u1, v1;
} UV_Coords;


typedef struct UV_Map UV_Map;
struct UV_Map {
	// used for normalization, ties this to one particular
	// resolution regardless...
	// But you can have the same instance attached to multiple
	// textures...
	i32          ncoords;
	// Keeping UV_Coords faster than smaller unnormalized integer coords?
	// Fetching, likely sporadic, what are the cache benefits regardless?
	UV_Coords     coords[];
};

typedef struct R_Texture R_Texture;
struct R_Texture {
	union {
		vec2i                resolution;
		vec2i                reso;
	};
	TextureFormat           format;
	// seems negligible to store this per texture,
	// easy sampling effects
	Matrix         color_transform;
	// todo: implement this?
	// texture has a white pixel, no need to swap textures then...
	vec2i              white_pixel;
	// attachments...
	UV_Map            *uv_map;
};

// typedef u64 RID;
typedef R_Texture *RID;


#define RID_NONE ((RID)(0))



typedef struct {
	vec2i  viewport;
	RID      output;
	RID     texture;
	i64     sampler;
	i64    topology;
	i64      shader;
	i64     blender;
} R_Draw_State;




typedef struct R_Renderer R_Renderer;
R_Renderer *R_InitRenderer(WID window);




// todo: remove this, used to be priviliged info...
#define R_GetTextureInfo(rend, rid) ((rid)->resolution)


enum {
	R_BIND_INPUT  = 1,
	R_BIND_OUTPUT = 2,
	R_DYNAMIC     = 4,
};

// maybe you should take an 'RID' to recycle an existing texture
RID R_InstallTextureEx(R_Renderer *rend, TextureFormat format, vec2i resolution, int flags, void *contents, i32 contentsstride);
RID R_InstallTexture(R_Renderer *rend, TextureFormat format, vec2i resolution, void *contents);
RID R_InstallSurface(R_Renderer *rend, TextureFormat format, vec2i resolution);
void R_UpdateTexture(R_Renderer *rend, RID rid, iRect region, Color *contents, int stride);

// call after end frame
void R_ReadWindowOutputData(R_Renderer *rend, u8 *memory, int stride);



typedef struct {
	i32 offset;
} R_SubmissionTicket;


RID R_GetWindowOutput(R_Renderer *rend);

R_SubmissionTicket R_SubmitVertices(R_Renderer *renderer, R_Vertex3 *vertices, i32 number);
R_Vertex3 *R_QueueVertices(R_Renderer *rend, i32 number);



void R_DrawVertices(R_Renderer *rend, R_SubmissionTicket offset, i32 number);
void R_FlushVertices(R_Renderer *rend);
void R_ResizeWindowTarget(R_Renderer *rend, vec2i reso);
void R_EndFrame(R_Renderer *rend);
void R_Synchronize(R_Renderer *rend);

void R_ClearSurface(R_Renderer *rend, Color color);

void R_SetVirtualReso(R_Renderer *rend, vec2i reso);
void R_SetColorTransform(R_Renderer *rend, Matrix transform);
void R_SetShader(R_Renderer *rend, ShaderId id);
void R_SetOutput(R_Renderer *rend, RID id);
void R_SetTexture(R_Renderer *rend, RID id);
void R_SetTopology(R_Renderer *rend, Topology id);
void R_SetSampler(R_Renderer *rend, SamplerId id);
void R_SetBlender(R_Renderer *rend, BlenderId id);
void R_SetViewport(R_Renderer *rend, vec2i size);

RID R_GetTexture(R_Renderer *rend);
RID R_GetSurface(R_Renderer *rend);
