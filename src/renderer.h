
typedef enum {
	FORMAT_NONE = 0,
	FORMAT_R8_UNORM,
	FORMAT_R8G8B8_UNORM,
	FORMAT_R32G32B32_FLOAT,
} TextureFormat;

typedef enum {
	MODE_NONE     ,
	TOPO_TRIANGLES,
	MODE_LINES    ,
} Topology;

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
	// todo: should none be the default texture?
	TEXTURE_NONE = 0,

	TEXTURE_DEFAULT,

	TEXTURE_RT_WINDOW,
	TEXTURE_RT_BASE,

	TEXTURE_FIRST_UNRESERVED_ID,

	TEXTURE_CAPACITY = 128,
} TextureId;

typedef enum {
	SHADER_NONE      =  0,
	SHADER_DEFAULT       ,

	SHADER_FIRST_UNRESERVED_ID,
	SHADER_CAPACITY  = 32,
} ShaderId;



typedef struct R_Renderer R_Renderer;

typedef struct {
	OS_Window_Handle window;
} Init_Renderer;



typedef struct {
	vec3    position;
	vec2   texcoords;
	Color      color;
	u8          _[8];
} R_Vertex3;

STATIC_ASSERT(!(sizeof(R_Vertex3) & 15));
STATIC_ASSERT(sizeof(R_Vertex3) == 32);

R_Renderer *R_InitRenderer(Init_Renderer init);

vec2i R_GetTextureInfo(R_Renderer *rend, TextureId id);
TextureId R_NewTextureId(R_Renderer *rend);

typedef struct {
	TextureId id;
	vec2i     resolution;
	TextureFormat  format;
	void     *colors;
	i32       stride;
	b32       is_input;
	b32       is_output;
} R_TEXTURE_PARAMS;

TextureId R_InstallTextureEx(R_Renderer *rend, R_TEXTURE_PARAMS args);
TextureId R_InstallTexture(R_Renderer *rend, TextureId id, TextureFormat format, vec2i resolution, void *contents);
TextureId R_InstallSurface(R_Renderer *rend, TextureId id, TextureFormat format, vec2i resolution);

typedef struct {
	i32 offset;
} R_SubmissionTicket;

R_SubmissionTicket R_SubmitVertices(R_Renderer *renderer, R_Vertex3 *vertices, i32 number);
void R_DrawVertices(R_Renderer *rend, R_SubmissionTicket offset, i32 number);


R_Vertex3 *R_QueueVertices(R_Renderer *rend, i32 number);
void R_FlushVertices(R_Renderer *rend);
void R_BeginFrame(R_Renderer *rend);
void R_EndFrame(R_Renderer *rend);

void R_ClearSurface(R_Renderer *rend, Color color);

void R_SetSurface(R_Renderer *rend, TextureId id);
void R_SetTexture(R_Renderer *rend, TextureId id);
void R_SetShader(R_Renderer *rend, ShaderId id);
void R_SetTopology(R_Renderer *rend, Topology id);
void R_SetSampler(R_Renderer *rend, SamplerId id);
void R_SetBlender(R_Renderer *rend, BlenderId id);
void R_SetViewport(R_Renderer *rend, vec2i resolution);
void R_SetViewportFullScreen(R_Renderer *rend);

TextureId R_GetTexture(R_Renderer *rend);
TextureId R_GetSurface(R_Renderer *rend);

void R_Blit(R_Renderer *rend, TextureId dest, TextureId src);
