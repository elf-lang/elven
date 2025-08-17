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



typedef u64 RID;

// these are converted into real handles once you pass them in
// into a renderer function that takes a handle
#define RID_NONE                 ((RID) 0)
#define RID_TEXTURE_DEFAULT      ((RID) 1)
#define RID_RENDER_TARGET_WINDOW ((RID) 2)

// we only use 3
#define RID_SPECIAL_RANGE        (4)
#define RID_SPECIAL_MASK         (RID_SPECIAL_RANGE - 1)
#define RID_FIRST_NONSPECIAL     ((RID) RID_SPECIAL_RANGE)


// todo: remove!
typedef enum {
	SHADER_NONE      =  0,
	SHADER_DEFAULT       ,

	SHADER_FIRST_UNRESERVED_ID,
	SHADER_CAPACITY  = 32,
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

R_Renderer *R_InitRenderer(OS_WindowId window);
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

typedef struct {
	i32 offset;
} R_SubmissionTicket;

R_SubmissionTicket R_SubmitVertices(R_Renderer *renderer, R_Vertex3 *vertices, i32 number);
R_Vertex3 *R_QueueVertices(R_Renderer *rend, i32 number);

void R_DrawVertices(R_Renderer *rend, R_SubmissionTicket offset, i32 number);
void R_FlushVertices(R_Renderer *rend);
void R_BeginFrame(R_Renderer *rend);
void R_EndFrame(R_Renderer *rend);

void R_ClearSurface(R_Renderer *rend, Color color);

void R_SetSurface(R_Renderer *rend, RID id);
void R_SetTexture(R_Renderer *rend, RID id);
void R_SetShader(R_Renderer *rend, ShaderId id);
void R_SetTopology(R_Renderer *rend, Topology id);
void R_SetSampler(R_Renderer *rend, SamplerId id);
void R_SetBlender(R_Renderer *rend, BlenderId id);
void R_SetViewport(R_Renderer *rend, vec2i resolution);
void R_SetViewportFullScreen(R_Renderer *rend);

RID R_GetTexture(R_Renderer *rend);
RID R_GetSurface(R_Renderer *rend);

// todo: weird! remove this from here, the renderer should't
// even be the one to determine how to blit the rect with proper
// aspect ratio!
iRect R_GetBlitRect(R_Renderer *rend, RID dest, RID src);
void R_Blit(R_Renderer *rend, RID dest, RID src);
