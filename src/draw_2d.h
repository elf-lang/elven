// todo: rename this file

typedef struct R_Renderer R_Renderer;

// todo: remove from here
typedef struct {
	vec4 rows[4];
} Matrix;

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


void D_PushMatrix();
void D_PopMatrix();
void D_LoadIdentity();

void D_DrawRectangle(R_Renderer *rend, f32 x, f32 y, f32 w, f32 h);
void D_DrawCircle(R_Renderer *rend, f32 x, f32 y, f32 r, f32 v);
void D_DrawLine(R_Renderer *rend, f32 x0, f32 y0, f32 x1, f32 y1);
void D_DrawTriangle(R_Renderer *rend, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2);
void D_SetTexture(R_Renderer *rend, TextureId id);
void D_SolidFill(R_Renderer *rend);
void D_Clear(R_Renderer *rend, Color color);


void D_SetAlpha(int a);
void D_SetColor0(Color color);
void D_SetColor1(Color color);
void D_SetColor2(Color color);
void D_SetColor3(Color color);
void D_SetColor(Color color);
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

void D_BeginQuads(R_Renderer *rend);
void D_PushQuad(R_Renderer *rend, Rect dst, iRect src);
void D_EndQuads(R_Renderer *rend);

int D_GetFont();
void D_SetFont(int font);
