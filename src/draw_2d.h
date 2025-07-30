// this is kind of silly,
// the idea is that it stores parameters that you may not
// want to pass in every time you call draw functions, so
// we keep this globally and you modify it thru functions.
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
} D_DrawBrush;

void D_BeginDrawing(jam_State *J);
void D_EndDrawing(jam_State *J);

void D_DrawRectangle(jam_State *J, f32 x, f32 y, f32 w, f32 h);
void D_DrawCircle(jam_State *J, f32 x, f32 y, f32 r, f32 v);
void D_DrawLine(jam_State *J, f32 x0, f32 y0, f32 x1, f32 y1);
void D_DrawTriangle(jam_State *J, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2);

void D_SetTexture(jam_State *J, TextureId id);
void D_SolidFill(jam_State *J);

void D_Clear(jam_State *J, Color color);

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
