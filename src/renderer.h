typedef enum {
	MODE_NONE     ,
	TOPO_TRIANGLES,
	MODE_LINES    ,
} Topology;

typedef struct R_Renderer R_Renderer;

typedef struct {
	vec3    position;
	vec2   texcoords;
	Color      color;
	u8          _[8];
} R_Vertex3;

STATIC_ASSERT(!(sizeof(R_Vertex3) & 15));
STATIC_ASSERT(sizeof(R_Vertex3) == 32);

R_Renderer *R_InitRenderer(OS_WindowId window);
vec2i R_GetTextureInfo(R_Renderer *rend, TextureId id);
TextureId R_FindFreeTexture(R_Renderer *rend);

typedef struct {
	vec2i     resolution;
	TextureFormat  format;
	void     *colors;
	i32       stride;
	b32       is_input;
	b32       is_output;
} R_TEXTURE_PARAMS;

void R_InstallTextureEx(R_Renderer *rend, TextureId id, R_TEXTURE_PARAMS args);
void R_InstallTexture(R_Renderer *rend, TextureId id, TextureFormat format, vec2i resolution, void *contents);
void R_InstallSurface(R_Renderer *rend, TextureId id, TextureFormat format, vec2i resolution);

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

// todo: weird!
iRect R_GetBlitRect(R_Renderer *rend, TextureId dest, TextureId src);
void R_Blit(R_Renderer *rend, TextureId dest, TextureId src);
