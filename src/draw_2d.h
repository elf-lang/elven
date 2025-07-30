
// this is kind of silly,
// the idea is that it stores parameters that we don't necessarily
// want to pass in every time you call a draw function.
// So we keep this globally and you modify it thru functions.
typedef struct {
	f32   rotation;
	vec2    center;
	vec2     scale;
	vec2    offset;
	Color  color_0;
	Color  color_1;
	Color  color_2;
	Color  color_3;
	struct { f32 x0, y0, x1, y1; } region;
	int    font;
	int    font_size;
	vec2i  fliponce;
	vec2   texture_inv_resolution;
} D_DrawBrush;

void D_BeginDrawing(R_Renderer *rend);
void D_EndDrawing(R_Renderer *rend);
void D_DrawRectangle(R_Renderer *rend, f32 x, f32 y, f32 w, f32 h);
void D_DrawCircle(R_Renderer *rend, f32 x, f32 y, f32 r, f32 v);
void D_DrawLine(R_Renderer *rend, f32 x0, f32 y0, f32 x1, f32 y1);
void D_DrawTriangle(R_Renderer *rend, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2);
void D_SetTexture(R_Renderer *rend, TextureId id);
void D_SolidFill(R_Renderer *rend);
void D_Clear(R_Renderer *rend, Color color);

void D_SetColor0(Color color);
void D_SetColor1(Color color);
void D_SetColor2(Color color);
void D_SetColor3(Color color);
void D_SetColor(Color color);
void D_SetRegion(i32 x0, i32 y0, i32 x1, i32 y1);
void D_SetCenter(f32, f32);
void D_SetOffset(f32, f32);
void D_SetScale(f32, f32);
void D_SetRotation(f32);
void D_SetFlipOnce(int flipx, int flipy);

Color D_GetColor0();

int D_GetFont();
void D_SetFont(int font);
void D_DrawText(R_Renderer *rend, f32 x, f32 y, char *text);
