//
// global state
// and rendering wrapper functions, try not to call the renderer's
// immediate mode API directly... instead go thru here!
//
// drawing parameters, if you are a functions that
// draws something, you may want to be affected by
// the drawing parameters specified here!
// For instance:
//
//	SetColor('RED')
//	DrawRectangle()
//	DrawCircle()
//	DrawTriangle()
//	DrawText()
//
//	All those functions are different, but they
// use the 'color' specified in the draw state.
//
//	Why do things this way? Good question.
//
//

// todo: remove from here
typedef struct {
	vec4 rows[4];
} Matrix;



typedef struct D_FONT D_FONT;
typedef D_FONT *FONT_HANDLE;


typedef struct R_Renderer R_Renderer;
typedef struct {
	R_Renderer *rend;

	OS_WindowId  window;
	i64          begin_cycle_clock;
	f64          clocks_to_seconds;
	f64          target_seconds_to_sleep;
	f64          pending_seconds_to_sleep;
	vec2         mouse;

	RID          targetsurface;
	RID          basesurface;

	// tiles lib
	struct {
		struct TileSet  *tileset;
		struct TileMap  *tilemap;
		int              layers_off;
	};

	union {
		Color          colors[4];
		struct {
			Color       color_0;
			Color       color_1;
			Color       color_2;
			Color       color_3;
		};
	};


	FONT_HANDLE font;
	int         font_size;

	vec2i       fliponce;
	vec2        texture_inv_resolution;

	// quad texture coordinates already pre-multiplied
	struct { f32 u0, v0, u1, v1; } region;

	Matrix   transform;
	Matrix   matrixstack[16];
	int      matrixindex;

	// todo: remove this probably...
	struct { Rect dst; iRect src; } quads[4096];
	int                            nquads;
} D_DrawState;



extern D_DrawState gd;



void D_PushMatrix();
void D_PopMatrix();
void D_LoadIdentity();


void D_SolidFill();
void D_SetTexture(RID id);

void D_SetAlpha(int alpha);

void D_SetLayerColor(int layer, Color color);
void D_SetLayerAlpha(int layer, int alpha);

void D_SetColor(Color color);
Color D_GetColor();

void D_SetColor0(Color color);
void D_SetColor1(Color color);
void D_SetColor2(Color color);
void D_SetColor3(Color color);

Color D_GetColor0();
Color D_GetColor1();
Color D_GetColor2();
Color D_GetColor3();

void D_SetRegion(i32 x0, i32 y0, i32 x1, i32 y1);


void D_SetFlipOnce(int flipx, int flipy);


void D_SetCenter(f32, f32);
void D_Translate(f32, f32);
void D_SetOffset(f32, f32);
void D_SetScale(f32, f32);
vec3 D_GetScale();
void D_SetRotation(f32);

void D_BeginQuads();
void D_PushQuad(Rect dst, iRect src);
void D_EndQuads();

FONT_HANDLE D_GetFont();
void D_SetFont(FONT_HANDLE font);


D_FONT *InstallFont(char *name, int size);
int UninstallFont(D_FONT *);
f32 MeasureText(const char *text);


void D_Clear(Color color);
void D_DrawRectangle(f32 x, f32 y, f32 w, f32 h);
void D_DrawCircle(f32 x, f32 y, f32 r, f32 v);
void D_DrawLine(f32 x0, f32 y0, f32 x1, f32 y1);
void D_DrawTriangle(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2);
void D_DrawText(f32 x, f32 y, const char *text);

int  A_InitAudio();
int  A_GetNumVoices();
int  A_GetVoiceSound(int voiceid);
void A_StopVoice(int id);
int  A_PlaySound(int soundid);
int  A_LoadSoundFromFile(int soundid, char *name);