
#include "d_draw.h"


void D_LoadIdentity();


static void D_InitWindow(const char *name, vec2i reso) {
	g_core.window = OS_InstallWindow(name, reso);
	g_core.r_window = R_CreateWindow(g_core.rend, g_core.window);
	g_core.output_target = R_GetWindowOutput(g_core.r_window);
	// so we just assume that if a base resolution is passed in
	// the user wants to draw to a fixed resolution render target
	if (reso.x != 0 && reso.y != 0) {
		g_core.base = R_CreateInputOutputTexture(g_core.rend, R_FORMAT_RGBA_U8, reso);
		TRACELOG("Fixed Res Output Target, %ix%i", reso.x, reso.y);
		g_core.output_target = g_core.base;
	}

	g_core.target_fps = 60;
	g_core.begin_cycle_clock = OS_GetTickCounter();

	D_LoadIdentity();
}

// Todo, switch param order
static rect_i32 GetScaledRectWithSameAspect(vec2i dst_r, vec2i src_r) {
	i32 max_scale = MIN(dst_r.x / (f32) src_r.x, dst_r.y / (f32) src_r.y);

	rect_i32 rect;
	rect.w = src_r.x * max_scale;
	rect.h = src_r.y * max_scale;
	rect.x = (dst_r.x - rect.w) * 0.5;
	rect.y = (dst_r.y - rect.h) * 0.5;
	return rect;
}

// todo: frame limiting is tied to polling the window...
// this is what you want for simplicity, but what if you want
// multiple windows?
static b32 D_PollWindow() {

	b32 res = OS_PollWindow(g_core.window);
	if (!res) goto _esc;

	vec2i window_reso = OS_GetWindowResolution(g_core.window);
	R_UpdateWindowTargets(g_core.rend, g_core.r_window, window_reso);

	// note that this also works for the case in which the output target
	// is the window, because it gets resized to match the window anyways
	// prior to this
	{
		vec2i output_reso = g_core.output_target->reso;
		rect_i32 output_rect = GetScaledRectWithSameAspect(window_reso, output_reso);
		vec2i mouse = OS_GetWindowMouse(g_core.window);
		g_core.mouse.x = 0 + (mouse.x - output_rect.x) / (f32) output_rect.w;
		g_core.mouse.y = 1 - (mouse.y - output_rect.y) / (f32) output_rect.h;
	}

	// todo: keep this cached!
	i64 ticks_per_second = OS_GetTicksPerSecond();
	i64 ticks_per_ms = ticks_per_second / 1000;
	i64 ticks_per_frame = ticks_per_second / g_core.target_fps;

	i64 end_cycle_clock = OS_GetTickCounter();


	i64 ticks_elapsed = end_cycle_clock - g_core.begin_cycle_clock;
	i64 ticks_to_sleep = ticks_per_frame - ticks_elapsed;

	// todo: I think the busy loop thing should be implemented per OS
	if (ticks_to_sleep > ticks_per_ms) {
		OS_Sleep(ticks_to_sleep / ticks_per_ms);
	}

	// transition into rapid eye movement sleep ...
	do {
		end_cycle_clock = OS_GetTickCounter();
	} while (end_cycle_clock - g_core.begin_cycle_clock < ticks_per_frame);
	g_core.begin_cycle_clock = end_cycle_clock;


	OS_ShowWindow(g_core.window);

	_esc:
	return res;
}


ELF_FUNCTION(L_InitWindow) {
	const char *name = elf_loadtext(S, 1);

	vec2i reso = {};
	if (nargs > 2) {
		reso.x = elf_loadint(S, 2);
		reso.y = elf_loadint(S, 3);
	}

	D_InitWindow(name, reso);

	return 0;
}

ELF_FUNCTION(L_PollWindow) {
	b32 res = D_PollWindow();
	elf_pushbool(S, res);
	return 1;
}

// todo: possibly rename to "Keyboard" and then MouseButton to "Mouse"
ELF_FUNCTION(L_Button) {
	int state = 0;
	for (int i = 1; i < nargs; i ++) {
		i32 index = elf_loadint(S, i);
		state |= OS_GetWindowKey(g_core.window, index);
	}
	elf_pushint(S, state);
	return 1;
}

ELF_FUNCTION(L_MouseButton) {
	i32 index = elf_loadint(S, 1);
	elf_pushint(S, OS_GetWindowKey(g_core.window, KEY_MOUSE_LEFT + index));
	return 1;
}

ELF_FUNCTION(L_GetMouseWheel) {
	elf_pushint(S, OS_GetWindowMouseWheel(g_core.window).y);
	return 1;
}

ELF_FUNCTION(L_GetMouseX) {
	elf_pushnum(S, g_core.mouse.x);
	return 1;
}

ELF_FUNCTION(L_GetMouseY) {
	elf_pushnum(S, g_core.mouse.y);
	return 1;
}

ELF_FUNCTION(L_GetFileDrop) {

	WID window = g_core.window;
	i32 index = elf_loadint(S, 1);
	elf_pushtext(S, OS_GetFileDrop(index));
	return 1;
}

ELF_FUNCTION(L_GetNumFileDrops) {
	WID window = g_core.window;
	elf_pushint(S, OS_GetNumFileDrops());
	return 1;
}

ELF_FUNCTION(L_OpenFileDialog) {

	const char *path = elf_loadtext(S, 1);

	char buffer[256] = {};
	int result = OS_OpenFileDialog((char *) path, buffer, sizeof(buffer));
	if (result) {
		elf_pushtext(S, buffer);
	} else {
		elf_pushnil(S);
	}
	return 1;
}


//
// VERTEX TRANSFORM:
//

ELF_FUNCTION(L_PushMatrix) {
	D_PushMatrix();
	return 0;
}

ELF_FUNCTION(L_PopMatrix) {
	D_PopMatrix();
	return 0;
}

ELF_FUNCTION(L_LoadIdentity) {
	D_LoadIdentity();
	return 0;
}

ELF_FUNCTION(L_SetRotation) {
	f32 radians = elf_loadnum(S, 1);
	D_SetRotation(radians);
	return 0;
}

ELF_FUNCTION(L_SetScale) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_SetScale(x, y);
	return 0;
}

ELF_FUNCTION(L_SetOffset) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_SetOffset(x, y);
	return 0;
}

ELF_FUNCTION(L_Translate) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_Translate(x, y);
	return 0;
}

//
// COLOR REGISTERS:
//

void D_SetColor(Color color) {
	g_core.color_0 = g_core.color_1 = color;
	g_core.color_2 = g_core.color_3 = color;
}

void D_SetAlpha(u32 alpha) {
	g_core.color_0.a = g_core.color_1.a = (u8) alpha;
	g_core.color_2.a = g_core.color_3.a = (u8) alpha;
}

void D_SetLayerColor(u32 layer, Color color) {
	g_core.colors[layer] = color;
}

void D_SetLayerAlpha(u32 layer, u32 alpha) {
	g_core.colors[layer].a = (u8) alpha;
}

static void SetColor0(Color color) { g_core.colors[0] = color; };
static void SetColor1(Color color) { g_core.colors[1] = color; };
static void SetColor2(Color color) { g_core.colors[2] = color; };
static void SetColor3(Color color) { g_core.colors[3] = color; };

void D_SolidFill() {
	g_core.texture_blend = 0.f;
}


ELF_FUNCTION(L_SetLayerColor) {
	int layer = elf_loadint(S, 1);
	Color color = _get_color_args2(S, 2, nargs);
	D_SetLayerColor(layer, color);
	return 0;
}

ELF_FUNCTION(L_SetLayerAlpha)  {
	int layer = elf_loadint(S, 1);
	int alpha = elf_loadint(S, 2);
	D_SetLayerAlpha(layer, alpha);
	return 0;
}

ELF_FUNCTION(L_SetColor) {
	D_SetColor(_get_color_arg(S, nargs));
	return 0;
}

ELF_FUNCTION(L_SetAlpha) {
	D_SetAlpha((u8) elf_loadint(S, 1));
	return 0;
}

ELF_FUNCTION(L_SetColor0) {
	SetColor0(_get_color_arg(S, nargs));
	return 0;
}

ELF_FUNCTION(L_SetColor1) {
	SetColor1(_get_color_arg(S, nargs));
	return 0;
}

ELF_FUNCTION(L_SetColor2) {
	SetColor2(_get_color_arg(S, nargs));
	return 0;
}

ELF_FUNCTION(L_SetColor3) {
	SetColor3(_get_color_arg(S, nargs));
	return 0;
}

ELF_FUNCTION(L_SetColorP) {
	u32 p = elf_loadint(S, 1);
	D_SetColor(COLOR_UNPACK(p));
	return 0;
}




void D_SetRegion(i32 x0, i32 y0, i32 x1, i32 y1) {
	g_core.region.u0 = x0 * g_core.texture_inv_resolution.x;
	g_core.region.v0 = y0 * g_core.texture_inv_resolution.y;
	g_core.region.u1 = x1 * g_core.texture_inv_resolution.x;
	g_core.region.v1 = y1 * g_core.texture_inv_resolution.y;
}

void D_SetRegionFromUVMap(i32 id) {
	RID t = D_GetTexture(&g_core.ctx);
	g_core.region.u0 = t->uv_map->coords[id].u0;
	g_core.region.v0 = t->uv_map->coords[id].v0;
	g_core.region.u1 = t->uv_map->coords[id].u1;
	g_core.region.v1 = t->uv_map->coords[id].v1;
}

// todo: this silly!
void D_SetFlipOnce(int x, int y) {
	g_core.mirrortextureonce.x = x;
	g_core.mirrortextureonce.y = y;
}

ELF_FUNCTION(L_SetViewport) {
	vec2i reso;
	reso.x = elf_loadint(S, 1);
	reso.y = elf_loadint(S, 2);
	D_SetViewport(&g_core.ctx, rect_i32_from_size(reso));
	return 0;
}

ELF_FUNCTION(L_SolidFill) {
	D_SolidFill();
	return 0;
}

ELF_FUNCTION(L_SetTexture) {
	RID id = (RID) elf_loadsys(S, 1);
	ASSERT(id != RID_NONE);
	D_SetTexture(&g_core.ctx, id);
	g_core.texture_blend = 1.f;
	return 0;
}

ELF_FUNCTION(L_SetShader) {
	R_Shader id = elf_loadint(S, 1);
	D_SetShader(&g_core.ctx, id);
	return 0;
}

ELF_FUNCTION(L_SetRegion) {
	if (nargs == 2) {
		D_SetRegionFromUVMap(elf_loadint(S, 1));
	}
	else {
		i32 x0 = elf_loadint(S, 1);
		i32 y0 = elf_loadint(S, 2);
		i32 x1 = x0 + elf_loadint(S, 3);
		i32 y1 = y0 + elf_loadint(S, 4);
		D_SetRegion(x0, y0, x1, y1);
	}
	return 0;
}

ELF_FUNCTION(L_SetFlipOnce) {
	int x = elf_loadint(S, 1);
	int y = elf_loadint(S, 2);
	D_SetFlipOnce(x, y);
	return 0;
}

ELF_FUNCTION(L_SetVirtualRes) {
	vec2 reso;
	reso.x = elf_loadnum(S, 1);
	reso.y = elf_loadnum(S, 2);
	D_SetOrthoTransform(&g_core.ctx, reso);
	return 0;
}

ELF_FUNCTION(L_SetOutput) {

	RID rid = (RID) elf_loadsys(S, 1);
	D_SetOutput(&g_core.ctx, rid);
	return 0;
}

// todo: rename to something like SetOutputToWindow()
ELF_FUNCTION(L_SetOutputWindow) {
	D_SetOutput(&g_core.ctx, R_GetWindowOutput(g_core.r_window));
	return 0;
}

//
// QUERY STATE INFO:
//

ELF_FUNCTION(L_GetOutputReso) {
	vec2i reso = g_core.output_target->reso;
	elf_pushint(S, reso.x);
	elf_pushint(S, reso.y);
	return 2;
}

// todo: deprecate!
ELF_FUNCTION(L_GetScreenW) {
	vec2i reso = g_core.output_target->reso;
	elf_pushint(S, reso.x);
	return 1;
}

// todo: deprecate!
ELF_FUNCTION(L_GetScreenH) {
	vec2i reso = g_core.output_target->reso;
	elf_pushint(S, reso.y);
	return 1;
}

//
// DRAWING:
//

static void ResetPerDrawState() {
	// Todo, place per draw state within the same struct to do one wipe
	g_core.mirrortextureonce.x = 0;
	g_core.mirrortextureonce.y = 0;
}

// todo: pass in a clear color?
// if the output rectangle is smaller, issue a clear with
// the target color?
// or who cares, always clear?
// todo: rename to blit to screen, why are we thinking in
// terms of windows at render time...
static void BlitToWindow(RID src) {
	RID dst = R_GetWindowOutput(g_core.r_window);
	ASSERT(src != dst);

	vec2i dst_r = dst->reso;
	vec2i src_r = src->reso;
	rect_i32 rect = GetScaledRectWithSameAspect(dst_r, src_r);

	vec2i xy = rect.pos;
	vec2i wh = rect.size;
	vec3 p0 = { xy.x + wh.x * 0, xy.y + wh.y * 0, 0 };
	vec3 p1 = { xy.x + wh.x * 0, xy.y + wh.y * 1, 0 };
	vec3 p2 = { xy.x + wh.x * 1, xy.y + wh.y * 0, 0 };
	vec3 p3 = { xy.x + wh.x * 1, xy.y + wh.y * 1, 0 };

	D_SetBlender(&g_core.ctx, BLEND_DISABLE);
	D_SetTexture(&g_core.ctx, src);
	D_SetOutput(&g_core.ctx, dst);
	D_SetTopology(&g_core.ctx, R_TOPO_TRIANGLES);
	D_PushVertex(&g_core.ctx, (R_Vertex) {p0, {0,1}, WHITE });
	D_PushVertex(&g_core.ctx, (R_Vertex) {p1, {0,0}, WHITE });
	D_PushVertex(&g_core.ctx, (R_Vertex) {p3, {1,0}, WHITE });
	D_PushVertex(&g_core.ctx, (R_Vertex) {p0, {0,1}, WHITE });
	D_PushVertex(&g_core.ctx, (R_Vertex) {p3, {1,0}, WHITE });
	D_PushVertex(&g_core.ctx, (R_Vertex) {p2, {1,1}, WHITE });
}



static void GIFCapture();

void D_Clear(Color color) {
	R_ClearOutput(g_core.rend, g_core.params.output, color);
}

void D_DrawRectangle(f32 x, f32 y, f32 w, f32 h) {

	// todo: weird that we do this?
	RID t = D_GetTexture(&g_core.ctx);
	if (w == 0) w = t->reso.x;
	if (h == 0) h = t->reso.y;

	f32 u0 = g_core.region.u0;
	f32 v0 = g_core.region.v0;
	f32 u1 = g_core.region.u1;
	f32 v1 = g_core.region.v1;

	// todo: uv transform matrix?
	f32 temp;
	if (g_core.mirrortextureonce.x) { temp = u0; u0 = u1, u1 = temp; }
	if (g_core.mirrortextureonce.y) { temp = v0; v0 = v1, v1 = temp; }

	D_SetTopology(&g_core.ctx, R_TOPO_TRIANGLES);
	D_PushVertex(&g_core.ctx, (R_Vertex){{ x + 0, y + 0 },{ u0, v1 }, g_core.color_0 });
	D_PushVertex(&g_core.ctx, (R_Vertex){{ x + 0, y + h },{ u0, v0 }, g_core.color_1 });
	D_PushVertex(&g_core.ctx, (R_Vertex){{ x + w, y + h },{ u1, v0 }, g_core.color_2 });
	D_PushVertex(&g_core.ctx, (R_Vertex){{ x + 0, y + 0 },{ u0, v1 }, g_core.color_0 });
	D_PushVertex(&g_core.ctx, (R_Vertex){{ x + w, y + h },{ u1, v0 }, g_core.color_2 });
	D_PushVertex(&g_core.ctx, (R_Vertex){{ x + w, y + 0 },{ u1, v1 }, g_core.color_3 });

	ResetPerDrawState();
}

void D_DrawLine(f32 x0, f32 y0, f32 x1, f32 y1) {
	// todo: hmmm... why?
	D_SolidFill();
	D_SetTopology(&g_core.ctx, R_TOPO_LINES);
	D_PushVertex(&g_core.ctx, (R_Vertex){{x0,y0},{0,0},g_core.color_0});
	D_PushVertex(&g_core.ctx, (R_Vertex){{x1,y1},{1,1},g_core.color_1});
	ResetPerDrawState();
}

void D_DrawTriangle(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2) {
	D_SetTopology(&g_core.ctx, R_TOPO_TRIANGLES);
	D_PushVertex(&g_core.ctx, (R_Vertex){{x0,y0},{0,0},g_core.color_0});
	D_PushVertex(&g_core.ctx, (R_Vertex){{x1,y1},{1,1},g_core.color_1});
	D_PushVertex(&g_core.ctx, (R_Vertex){{x2,y2},{1,1},g_core.color_2});
	ResetPerDrawState();
}

// the greater v is, the lower quality the approximation is.
// v represents the distance from the midpoint of the approximation
// chord to the midpoint of the arch.
//
// Determine line vs arc approximation error, greatest
// deviation happens at the midpoint
//
// error = r - r * cos(angle * 0.5)
//
// Solve for angle:
//
// angle = 2 acos(1 - error / r)
//
// todo: allow for inner color and outer color
// todo: use all the color registers
void D_DrawCircle(f32 x, f32 y, f32 r, f32 v) {
	// Which chord sizes minimizes v?
	f32 chordsize = 2 * acosf(1 - v / r);
	i32 nchords = TAU / chordsize;
	Color color = g_core.color_0;
	D_SetTopology(&g_core.ctx, R_TOPO_TRIANGLES);
	for (i32 i = 0; i < nchords; i ++) {
		D_PushVertex(&g_core.ctx, (R_Vertex){{x,y},{0,0},color});
		f32 a = ((i + 0.0) / (f32) nchords) * TAU;
		f32 ax = x + cosf(a) * r;
		f32 ay = y + sinf(a) * r;
		D_PushVertex(&g_core.ctx, (R_Vertex){{ax,ay},{0,1},color});
		f32 b = ((i + 1.0) / (f32) nchords) * TAU;
		f32 bx = x + cosf(b) * r;
		f32 by = y + sinf(b) * r;
		D_PushVertex(&g_core.ctx, (R_Vertex){{bx,by},{1,1},color});
	}
	ResetPerDrawState();
}

ELF_FUNCTION(L_DrawTriangle) {
	f32 x0 = elf_loadnum(S, 1);
	f32 y0 = elf_loadnum(S, 2);

	f32 x1 = elf_loadnum(S, 3);
	f32 y1 = elf_loadnum(S, 4);

	f32 x2 = elf_loadnum(S, 5);
	f32 y2 = elf_loadnum(S, 6);

	D_DrawTriangle(x0, y0, x1, y1, x2, y2);
	return 0;
}

ELF_FUNCTION(L_DrawLine) {
	f32 x0 = elf_loadnum(S, 1);
	f32 y0 = elf_loadnum(S, 2);
	f32 x1 = elf_loadnum(S, 3);
	f32 y1 = elf_loadnum(S, 4);

	D_DrawLine(x0, y0, x1, y1);
	return 0;
}

ELF_FUNCTION(L_DrawCircle) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	f32 r = elf_loadnum(S, 3);

	f32 v = 1.0;
	if (nargs > 4) {
		v = elf_loadnum(S, 4);
	}

	D_DrawCircle(x, y, r, v);
	return 0;
}

ELF_FUNCTION(L_DrawRectangle) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	f32 w = elf_loadnum(S, 3);
	f32 h = elf_loadnum(S, 4);

	D_DrawRectangle(x, y, w, h);
	return 0;
}

ELF_FUNCTION(L_DrawRectOutline) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	f32 w = elf_loadnum(S, 3);
	f32 h = elf_loadnum(S, 4);
	//	D_DrawLine(x  , y-1, x   + w, y-1 + 1);
	//	D_DrawLine(x  , y+h, x   + w, y+h + 1);
	//	D_DrawLine(x-1, y  , x-1 + 1, y   + h);
	//	D_DrawLine(x+w, y  , x+w + 1, y   + h);

	D_DrawLine(x,y, x+w, y);
	D_DrawLine(x, y+h, x+w, y+h);
	D_DrawLine(x,y, x, y+h);
	D_DrawLine(x+w, y, x+w, y+h);
	return 0;
}

ELF_FUNCTION(L_EndDrawing) {

	// Generate Blit To Window Pass
	RID window_target = R_GetWindowOutput(g_core.r_window);

	if (g_core.output_target != window_target) {
		BlitToWindow(g_core.output_target);
	}

	// Flush Last Pass
	D_EndPass(&g_core.ctx);

	// Draw
	R_DrawParams draw_params = {
		.passes = g_core.passes,
		.num_passes = g_core.num_passes,
		.verts = g_core.verts,
		.verts_size_in_bytes = g_core.num_verts * sizeof(*g_core.verts),
	};
	R_Draw(g_core.rend, draw_params);

	// Present
	R_PresentWindow(g_core.rend, g_core.r_window);

	g_core.params.output = 0;

	if (g_core.gif_capture) {
		GIFCapture();
	}
	return 0;
}

ELF_FUNCTION(L_BeginDrawing) {
	D_BeginDrawing(&g_core.ctx);

	// Note, sets the viewport, and ortho transform ...
	D_SetOutput(&g_core.ctx, g_core.output_target);

	D_SolidFill();
	D_SetSampler(&g_core.ctx, R_SAMPLER_POINT);
	D_SetTopology(&g_core.ctx, R_TOPO_TRIANGLES);
	D_SetBlender(&g_core.ctx, R_BLENDER_ALPHA_BLEND);
	D_SetShader(&g_core.ctx, R_SHADER_SIMPLE);

	g_core.texture_blend = 1.f;
	// g_core.params.mixer.rows[0] = (vec4){ 1, 0, 0, 0 };
	// g_core.params.mixer.rows[1] = (vec4){ 0, 1, 0, 0 };
	// g_core.params.mixer.rows[2] = (vec4){ 0, 0, 1, 0 };
	// g_core.params.mixer.rows[3] = (vec4){ 0, 0, 0, 1 };

	D_SetColor(WHITE);
	return 0;
}

ELF_FUNCTION(L_Clear) {
	Color color = _get_color_arg(S, nargs);
	D_Clear(color);
	return 0;
}

ELF_FUNCTION(L_BlitToWindow) {
	RID src = (RID) elf_loadsys(S, 1);
	BlitToWindow(src);
	return 0;
}



//
// CAPTURING:
//	todo: this is a really high level api, make more granular in the future!
//

static void GIFCapture() {
	RID window = R_GetWindowOutput(g_core.r_window);
	Color *data = malloc(sizeof(*data) * window->reso.x * window->reso.y);
	__debugbreak();
	// R_ReadWindowOutputData(g_core.rend, (u8 *) data, window->reso.x * sizeof(*data));
	i32 centis = ceil(100.0 / g_core.target_fps);
 	msf_gif_frame(&g_core.gif_state, (u8 *) data, centis, 16, window->reso.x * 4);
 	free(data);
}


ELF_FUNCTION(L_BeginGIFCapture) {
	ASSERT(!g_core.gif_capture);
	RID window = R_GetWindowOutput(g_core.r_window);
	msf_gif_begin(&g_core.gif_state, window->reso.x, window->reso.y);
	g_core.gif_capture = true;
	return 0;
}

ELF_FUNCTION(L_EndGIFCapture) {
	const char *name = elf_loadtext(S, 1);
	ASSERT(g_core.gif_capture);
	MsfGifResult result = msf_gif_end(&g_core.gif_state);
	if (result.data) {
		FILE_HANDLE io = sys_open_file(name, SYS_OPEN_WRITE, SYS_CREATE_ALWAYS);
		sys_write_file(io, result.data, result.dataSize);
		sys_close_file(io);
	}
	msf_gif_free(result);
	return 0;
}

// todo: get an image!
ELF_FUNCTION(L_TakeScreenshot) {
	// const char *name = elf_loadtext(S, 1);
	// RID window = R_GetWindowOutput(g_core.r_window);
	// Color *data = malloc(sizeof(*data) * window->reso.x * window->reso.y);
	// R_ReadWindowOutputData(g_core.rend, (u8 *) data, window->reso.x * sizeof(*data));
	// // todo:
	// for (u32 i = 0; i < window->reso.x * window->reso.y; ++ i) {
	// 	data[i].a = 255;
	// }
	// stbi_write_bmp(name, window->reso.x, window->reso.y, 4, data);
	return 0;
}

//
// TEXTURES:
//
// todo: it'd be nice if we got some form of type safety!
//	todo: possibly just wrap in an object, but since elf
// changes quite a bit, keeping things as functions is safer
// long-term, wrappers can be made easily...
//
// todo: format strings (rgba_u8)
//
//


static inline RID _load_texture(elf_State *S, int index) {
	return (RID) elf_loadsys(S, index);
}



ELF_FUNCTION(L_NewOutputTexture) {
	vec2i reso;
	reso.x = elf_loadint(S, 1);
	reso.y = elf_loadint(S, 2);

	RID rid = R_CreateInputOutputTexture(g_core.rend, R_FORMAT_RGBA_U8, reso);
	elf_pushsys(S, (elf_Handle) rid);
	return 1;
}

ELF_FUNCTION(L_NewTexture) {
	vec2i resolution;
	_load_vec2i(S, 1, nargs, &resolution);

	RID rid = R_CreateTexture(g_core.rend, R_FORMAT_RGBA_U8, resolution, 0, 0);
	elf_pushsys(S, (elf_Handle) rid);
	elf_pushint(S, resolution.x);
	elf_pushint(S, resolution.y);
	return 3;
}

ELF_FUNCTION(L_LoadTextureFromImage) {
	Image *img = load_image(S, 1);

	RID rid = R_CreateTexture(g_core.rend, R_FORMAT_RGBA_U8, img->reso, 0, img->data);
	elf_pushsys(S, (elf_Handle) rid);
	elf_pushint(S, img->reso.x);
	elf_pushint(S, img->reso.y);
	return 3;
}

ELF_FUNCTION(L_LoadTexture) {
	const char *name = elf_loadtext(S, 1);

	i32 num_channels;
	i32 wanted_channels = 4;

	vec2i reso;
	u8 *colors = stbi_load(name, &reso.x, &reso.y, &num_channels, wanted_channels);

	if (colors) {

		RID rid = R_CreateTexture(g_core.rend, R_FORMAT_RGBA_U8, reso, 0, colors);
		elf_pushsys(S, (elf_Handle) rid);
		elf_pushint(S, reso.x);
		elf_pushint(S, reso.y);

		free(colors);
		return 3;
	}
	else {
		elf_pushnil(S);
		return 1;
	}
}

ELF_FUNCTION(L_AttachUVMap) {
	RID id = (RID) elf_loadsys(S, 1);
	i32 ncoords = elf_loadint(S, 2);

	UV_Map *uv_map = calloc(1, sizeof(*uv_map) + sizeof(*uv_map->coords) * ncoords);
	uv_map->ncoords = ncoords;

	id->uv_map = uv_map;
	return 0;
}

ELF_FUNCTION(L_SetTextureRegion) {
	RID texture = (RID) elf_loadsys(S, 1);

	i32 id = elf_loadint(S, 2);

	i32 x = elf_loadint(S, 3);
	i32 y = elf_loadint(S, 4);
	i32 w = elf_loadint(S, 5);
	i32 h = elf_loadint(S, 6);

	UV_Map *map = texture->uv_map;
	ASSERT(id >= 0 && id < map->ncoords);

	f32 inv_w = 1.0f / texture->reso.x;
	f32 inv_h = 1.0f / texture->reso.y;
	UV_Coords coords;
	coords.u0 = (x + 0) * inv_w;
	coords.v0 = (y + 0) * inv_h;
	coords.u1 = (x + w) * inv_w;
	coords.v1 = (y + h) * inv_h;

	map->coords[id] = coords;
	return 0;
}

ELF_FUNCTION(L_GetTextureInfo) {
	RID id = (RID) elf_loadsys(S, 1);
	vec2i reso = id->reso;

	elf_pushtab(S);

	elf_pushtext(S, "x");
	elf_pushint(S, reso.x);
	elf_setfield(S);

	elf_pushtext(S, "y");
	elf_pushint(S, reso.y);
	elf_setfield(S);
	return 1;
}

static void CopyImageToTexture(RID dst, vec2i dst_v, Image *src, rect_i32 src_r) {
	// todo: take elf state here in case the assertion fails...
	ASSERT(src_r.x>=0 && src_r.x+src_r.w<=src->reso.x);
	ASSERT(src_r.y>=0 && src_r.y+src_r.h<=src->reso.y);
	ASSERT(dst_v.x>=0 && dst_v.x+src_r.w<=dst->reso.x);
	ASSERT(dst_v.y>=0 && dst_v.y+src_r.h<=dst->reso.y);

	rect_i32 dst_r = (rect_i32){ dst_v.x, dst_v.y, src_r.w, src_r.h };

	i32 stride = src->reso.x * sizeof(*src->data);
	R_UpdateTexture(g_core.rend, dst, dst_r, stride, src->data + src_r.x + src_r.y * stride);
}

ELF_FUNCTION(L_CopyImageToTexture) {
	int index = 1;
	RID dst_t = _load_texture(S, index ++);

	vec2i dst_v;
	index += _load_vec2i(S, index, nargs, &dst_v);

	Image *src_i = (Image *) elf_loadsys(S, index ++);

	rect_i32 src_r = { 0, 0, src_i->reso.x, src_i->reso.y };
	if (nargs - index >= 4) {
		index += _load_irect(S, index, nargs, &src_r);
	}


	// rect_i32 dst_r = (rect_i32){ dst_v.x, dst_v.y, src_r.w, src_r.h };

	CopyImageToTexture(dst_t, dst_v, src_i, src_r);
	// R_UpdateTexture(g_core.rend, dst_t, dst_r
	// , src_i->data + src_r.x + src_r.y * src_i->reso.x
	// , src_i->reso.x * sizeof(*src_i->data));
	return 0;
}










elf_Binding l_state[] = {
	{ "InitWindow"                       , L_InitWindow                           },
	{ "PollWindow"                       , L_PollWindow                           },
	{ "GetFileDrop"                      , L_GetFileDrop                          },
	{ "GetNumFileDrops"                  , L_GetNumFileDrops                      },
	{ "OpenFileDialog"                   , L_OpenFileDialog                       },
	{ "Button"                           , L_Button                               },
	{ "MouseButton"                      , L_MouseButton                          },
	{ "GetMouseWheel"                    , L_GetMouseWheel                        },
	{ "GetMouseX"                        , L_GetMouseX                            },
	{ "GetMouseY"                        , L_GetMouseY                            },


	// TEXTURE
	{ "NewOutputTexture"                 , L_NewOutputTexture                     },
	{ "LoadTexture"                      , L_LoadTexture                          },
	{ "NewTexture"                       , L_NewTexture                           },
	{ "LoadTextureFromImage"             , L_LoadTextureFromImage                 },
	{ "CopyImageToTexture"               , L_CopyImageToTexture                   },
	{ "GetTextureInfo"                   , L_GetTextureInfo                       },
	{ "AttachUVMap"                      , L_AttachUVMap                          },
	{ "SetTextureRegion"                 , L_SetTextureRegion                     },

	// STATE
	{ "GetScreenW"                       , L_GetScreenW                           },
	{ "GetScreenH"                       , L_GetScreenH                           },
	{ "SetVirtualRes"                    , L_SetVirtualRes                        },
	{ "SetOutput"                        , L_SetOutput                            },
	{ "SetOutputWindow"                  , L_SetOutputWindow                      },

	{ "PushMatrix"                       , L_PushMatrix                           },
	{ "PopMatrix"                        , L_PopMatrix                            },
	{ "LoadIdentity"                     , L_LoadIdentity                         },
	{ "SetScale"                         , L_SetScale                             },
	{ "SetOffset"                        , L_SetOffset                            },
	{ "SetRotation"                      , L_SetRotation                          },
	{ "Translate"                        , L_Translate                            },

	{ "SetViewport"                      , L_SetViewport                          },
	{ "SetTexture"                       , L_SetTexture                           },
	{ "SetShader"                        , L_SetShader                            },
	{ "SetRegion"                        , L_SetRegion                            },
	{ "SetLayerColor"                    , L_SetLayerColor                        },
	{ "SetLayerAlpha"                    , L_SetLayerAlpha                        },
	{ "SetAlpha"                         , L_SetAlpha                             },
	{ "SetColorP"                        , L_SetColorP                            },
	{ "SetColor"                         , L_SetColor                             },
	{ "SetColor0"                        , L_SetColor0                            },
	{ "SetColor1"                        , L_SetColor1                            },
	{ "SetColor2"                        , L_SetColor2                            },
	{ "SetColor3"                        , L_SetColor3                            },
	{ "SolidFill"                        , L_SolidFill                            },
	{ "SetFlipOnce"                      , L_SetFlipOnce                          },

	// DRAWING
	{ "Clear"                            , L_Clear                                },
	{ "BlitToWindow"                     , L_BlitToWindow                         },
	{ "BeginDrawing"                     , L_BeginDrawing                         },
	{ "EndDrawing"                       , L_EndDrawing                           },
	{ "DrawRectangle"                    , L_DrawRectangle                        },
	{ "DrawRectOutline"                  , L_DrawRectOutline                      },
	{ "DrawTriangle"                     , L_DrawTriangle                         },
	{ "DrawLine"                         , L_DrawLine                             },
	{ "DrawCircle"                       , L_DrawCircle                           },
	{ "DrawCircle"                       , L_DrawCircle                           },

	{ "TakeScreenshot"                   , L_TakeScreenshot                       },
	{ "BeginGIFCapture"                  , L_BeginGIFCapture                      },
	{ "EndGIFCapture"                    , L_EndGIFCapture                        },
};
