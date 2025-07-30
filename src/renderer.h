
typedef struct R_Renderer R_Renderer;

typedef struct {
	OS_Window_Handle window;
} Init_Renderer;

typedef struct {
	i32 offset;
} R_Ticket;

typedef struct {
	vec2     position;
	vec2    texcoords;
	Color       color;
	u8    padding[12];
} R_VERTEX_2D;

STATIC_ASSERT(!(sizeof(R_VERTEX_2D) & 15));


enum {
	STATE_OUTPUT,
	STATE_INPUT,
	STATE_SAMPLER,
	STATE_TOPOLOGY,
	STATE_SHADER,
	STATE_INT_COUNT,
};

typedef struct R_Draw_State {
	vec2i            viewport;
	union {
		// todo: these don't even have to be 32-bit integers
		i32           states[STATE_INT_COUNT];
		struct {
			TextureId  output;
			TextureId  texture;
			SamplerId  sampler;
			Topology   topology;
			ShaderId   shader;
		};
	};
} R_Draw_State;



R_Renderer *R_InitRenderer(Init_Renderer init);

vec2i R_GetTextureInfo(R_Renderer *rend, TextureId id);
TextureId R_NewTextureId(R_Renderer *rend);

typedef struct {
	TextureId id;
	vec2i     resolution;
	R_FORMAT  format;
	void     *colors;
	i32       stride;
	b32       is_input;
	b32       is_output;
} R_TEXTURE_PARAMS;

TextureId R_InstallTextureEx(R_Renderer *rend, R_TEXTURE_PARAMS args);
TextureId R_InstallTexture(R_Renderer *rend, TextureId id, R_FORMAT format, vec2i resolution, void *contents);
TextureId R_InstallSurface(R_Renderer *rend, TextureId id, R_FORMAT format, vec2i resolution);

void R_DrawVertices(R_Renderer *rend, R_Ticket offset, i32 number);
R_Ticket R_SubmitVertices(R_Renderer *renderer, R_VERTEX_2D *vertices, i32 number);

R_VERTEX_2D *R_QueueVertices(R_Renderer *rend, i32 number);
void R_FlushVertices(R_Renderer *rend);
void R_BeginFrame(R_Renderer *rend);
void R_EndFrame(R_Renderer *rend);

void R_ClearSurface(R_Renderer *rend, Color color);

void R_SetSurface(R_Renderer *rend, TextureId id);
void R_SetTexture(R_Renderer *rend, TextureId id);
void R_SetShader(R_Renderer *rend, ShaderId id);
void R_SetTopology(R_Renderer *rend, Topology id);
void R_SetSampler(R_Renderer *rend, SamplerId id);
void R_SetViewport(R_Renderer *rend, vec2i resolution);
void R_SetViewportFullScreen(R_Renderer *rend);

TextureId R_GetTexture(R_Renderer *rend);
TextureId R_GetSurface(R_Renderer *rend);

void R_GetPrevDrawState(R_Renderer *rend, R_Draw_State *state);
void R_GetProxDrawState(R_Renderer *rend, R_Draw_State *state);

void R_Blit(R_Renderer *rend, TextureId dest, TextureId src);
