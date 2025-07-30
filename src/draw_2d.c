
#include "include/elf.h"
#include "src/core/r_core.h"
#include "src/internal_utils.h"
#include "src/r_auxilary.h"


#include "elements.h"
#include "draw.h"
#include "platform.h"
#include "renderer.h"
#include "stb_truetype.h"
#include "miniaudio.h"
#include "jam.h"
#include "draw_2d.h"

#include <stdlib.h>
#include <math.h>

global D_DrawBrush g;

void D_EndDrawing(jam_State *J) {
	R_Renderer *rend = J->rend;

	R_Blit(rend, TEXTURE_RT_WINDOW, TEXTURE_RT_BASE);
	R_EndFrame(rend);
}

void D_BeginDrawing(jam_State *J) {
	R_Renderer *rend = J->rend;

	R_BeginFrame(rend);

	// todo: just have a default brush
	D_SetScale(1, 1);
	D_SetOffset(0, 0);
	D_SetCenter(0, 0);
	D_SetTexture(J, TEXTURE_DEFAULT);

	R_SetSurface(rend, TEXTURE_RT_BASE);
	R_SetSampler(rend, SAMPLER_POINT);
	R_SetViewportFullScreen(rend);
	R_SetTopology(rend, TOPO_TRIANGLES);
}


void D_Clear(jam_State *J, Color color) {
	R_ClearSurface(J->rend, color);
}


void D_SetTexture(jam_State *J, TextureId id) {
	R_Renderer *rend = J->rend;
	R_SetTexture(rend, id);

	vec2i resolution = R_GetTextureInfo(rend, id);
	g.region.x0 = 0;
	g.region.y0 = 0;
	g.region.x1 = resolution.x;
	g.region.y1 = resolution.y;
}

void D_SolidFill(jam_State *J) {
	D_SetTexture(J, TEXTURE_DEFAULT);
}

void D_SetColor0(Color color) { g.color_0 = color; }
void D_SetColor1(Color color) { g.color_1 = color; }
void D_SetColor2(Color color) { g.color_2 = color; }
void D_SetColor3(Color color) { g.color_3 = color; }

void D_SetColor(Color color) {
	g.color_0 = g.color_1 = g.color_2 = g.color_3 = color;
}

void D_SetScale(f32 x, f32 y) {
	g.scale.x = x;
	g.scale.y = y;
}

void D_SetOffset(f32 x, f32 y) {
	g.offset.x = x;
	g.offset.y = y;
}

void D_SetCenter(f32 x, f32 y) {
	g.center.x = x;
	g.center.y = y;
}

void D_SetRegion(i32 x0, i32 y0, i32 x1, i32 y1) {
	g.region.x0 = x0;
	g.region.y0 = y0;
	g.region.x1 = x1;
	g.region.y1 = y1;
}

void D_SetRotation(f32 x) {
	g.rotation = x;
}

void D_DrawRectangle(jam_State *J, f32 x, f32 y, f32 w, f32 h) {

	R_SetTopology(J->rend, TOPO_TRIANGLES);

	x += g.offset.x;
	y += g.offset.y;

	vec2 r_p0 = { x + 0, y + 0 };
	vec2 r_p1 = { x + 0, y + h };
	vec2 r_p2 = { x + w, y + h };
	vec2 r_p3 = { x + w, y + 0 };

	//
	// todo: get this from the currently bound sampling window,
	// also, whenever this is set, allow for flipping of the
	// sampling region too.
	//
	vec2i resolution = R_GetTextureInfo(J->rend, R_GetTexture(J->rend));
	f32 inv_resolution_x = 1.0 / resolution.x;
	f32 inv_resolution_y = 1.0 / resolution.y;

	r_i32 src_r = {0, 0, 1, 1};
	f32 u0 = g.region.x0 * inv_resolution_x;
	f32 v0 = g.region.y0 * inv_resolution_y;
	f32 u1 = g.region.x1 * inv_resolution_x;
	f32 v1 = g.region.y1 * inv_resolution_y;

	R_VERTEX_2D *vertices = R_QueueVertices(J->rend, 6);
	vertices[0]=(R_VERTEX_2D){r_p0,{u0,v1},g.color_0};
	vertices[1]=(R_VERTEX_2D){r_p1,{u0,v0},g.color_1};
	vertices[2]=(R_VERTEX_2D){r_p2,{u1,v0},g.color_2};
	vertices[3]=(R_VERTEX_2D){r_p0,{u0,v1},g.color_0};
	vertices[4]=(R_VERTEX_2D){r_p2,{u1,v0},g.color_2};
	vertices[5]=(R_VERTEX_2D){r_p3,{u1,v1},g.color_3};
}

void D_DrawCircle(jam_State *J, f32 x, f32 y, f32 r, f32 v) {

	R_SetTopology(J->rend, TOPO_TRIANGLES);

	// todo: how do you actually make this proper...
	f32 circumference = TAU * r;
	i32 num_triangles = circumference / v;

	// todo: allow for inner color and outer color
	Color color = g.color_0;

	R_VERTEX_2D *vertices = R_QueueVertices(J->rend, num_triangles * 3);

	for (i32 i = 0; i < num_triangles; i ++) {
		i32 t = i * 3;
		vertices[t + 0] = (R_VERTEX_2D){{x,y},{0,0},color};

		f32 a = ((i + 0.0) / (f32) num_triangles) * TAU;
		f32 ax = x + cos(a) * r;
		f32 ay = y + sin(a) * r;
		vertices[t + 1] = (R_VERTEX_2D){{ax,ay},{0,1},color};

		f32 b = ((i + 1.0) / (f32) num_triangles) * TAU;
		f32 bx = x + cos(b) * r;
		f32 by = y + sin(b) * r;
		vertices[t + 2] = (R_VERTEX_2D){{bx,by},{1,1},color};
	}
}

void D_DrawLine(jam_State *J, f32 x0, f32 y0, f32 x1, f32 y1) {

	R_SetTexture(J->rend, TEXTURE_DEFAULT);
	R_SetTopology(J->rend, MODE_LINES);

	R_VERTEX_2D *vertices = R_QueueVertices(J->rend, 2);
	vertices[0] = (R_VERTEX_2D){{x0,y0},{0,0},g.color_0};
	vertices[1] = (R_VERTEX_2D){{x1,y1},{1,1},g.color_1};
}

void D_DrawTriangle(jam_State *J, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2) {
	R_SetTopology(J->rend, TOPO_TRIANGLES);

	R_VERTEX_2D *vertices = R_QueueVertices(J->rend, 3);
	vertices[0] = (R_VERTEX_2D){{x0,y0},{0,0},g.color_0};
	vertices[1] = (R_VERTEX_2D){{x1,y1},{1,1},g.color_1};
	vertices[2] = (R_VERTEX_2D){{x2,y2},{1,1},g.color_2};
}
