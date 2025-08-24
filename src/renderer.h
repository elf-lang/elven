// include this header in stuff that requires rendering

typedef enum {
	FORMAT_NONE = 0,
	FORMAT_R8_UNORM,
	FORMAT_R8G8B8_UNORM,
	FORMAT_R32G32B32_FLOAT,
} TextureFormat;

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

#define RID_NONE ((RID)(0))
typedef u64 RID;

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


typedef struct R_Renderer R_Renderer;

R_Renderer *R_InitRenderer(WID window);
vec2i R_GetTextureInfo(R_Renderer *rend, RID id);

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



typedef struct {
	i32 offset;
} R_SubmissionTicket;


RID R_GetWindowOutput(R_Renderer *rend);

R_SubmissionTicket R_SubmitVertices(R_Renderer *renderer, R_Vertex3 *vertices, i32 number);
R_Vertex3 *R_QueueVertices(R_Renderer *rend, i32 number);



void R_DrawVertices(R_Renderer *rend, R_SubmissionTicket offset, i32 number);
void R_FlushVertices(R_Renderer *rend);
void R_BeginFrame(R_Renderer *rend);
void R_EndFrame(R_Renderer *rend);
void R_Synchronize(R_Renderer *rend);

void R_ClearSurface(R_Renderer *rend, Color color);

void R_SetShader(R_Renderer *rend, ShaderId id);
void R_SetOutput(R_Renderer *rend, RID id);
void R_SetTexture(R_Renderer *rend, RID id);
void R_SetTopology(R_Renderer *rend, Topology id);
void R_SetSampler(R_Renderer *rend, SamplerId id);
void R_SetBlender(R_Renderer *rend, BlenderId id);
void R_SetViewport(R_Renderer *rend, vec2i size);
void R_SetVirtualReso(R_Renderer *rend, vec2i reso);

RID R_GetTexture(R_Renderer *rend);
RID R_GetSurface(R_Renderer *rend);
