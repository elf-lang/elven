#include "elements.h"
#include "platform.h"
#include "draw_2d.h"
#include "renderer.h"

// sin,cos
#include <math.h>

struct {

	Matrix transform;
	Matrix matrixstack[16];
	int    matrixindex;

	// todo: remove!
	vec2       center;
	vec2       offset;
	vec2        scale;

	Color     color_0;
	Color     color_1;
	Color     color_2;
	Color     color_3;
	int       font;
	int       font_size;
	vec2i     fliponce;
	vec2      texture_inv_resolution;
	// quad texture coordinates already pre-multiplied
	struct { f32 u0, v0, u1, v1; } region;

	struct { Rect dst; iRect src; } quads[4096];
	int                            nquads;
} g;


static Matrix ScaleMatrix(vec3 xyz) {
	Matrix c = {
		xyz.x,     0,     0, 0,
		    0, xyz.y,     0, 0,
		    0,     0, xyz.z, 0,
		    0,     0,     0, 1,
	};
	return c;
}
static Matrix RotationMatrix(f32 r) {
	f32 co = cosf(r);
	f32 si = sinf(r);
	Matrix c = {
		co, si, 0, 0,
		si, co, 0, 0,
		0,   0, 1, 0,
		0,   0, 0, 1,
	};
	return c;
}
static Matrix MultiplyMatrices(Matrix a, Matrix b) {
	Matrix c;
	for (int i = 0; i < 4; i ++) {
		for (int j = 0; j < 4; j ++) {
			c.rows[i].xyzw[j] =
			a.rows[i].xyzw[0] * b.rows[0].xyzw[j] +
			a.rows[i].xyzw[1] * b.rows[1].xyzw[j] +
			a.rows[i].xyzw[2] * b.rows[2].xyzw[j] +
			a.rows[i].xyzw[3] * b.rows[3].xyzw[j];
		}
	}
	return c;
}
static vec4 MultiplyMatrixVector(Matrix a, vec4 b) {
	vec4 c;
	for (int i = 0; i < 4; i ++) {
		c.xyzw[i] =
		b.xyzw[i] * a.rows[i].xyzw[0] +
		b.xyzw[i] * a.rows[i].xyzw[1] +
		b.xyzw[i] * a.rows[i].xyzw[2] +
		b.xyzw[i] * a.rows[i].xyzw[3];
	}
	return c;
}

void D_LoadIdentity() {
	Matrix id = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	g.transform = id;
}


void D_PushMatrix() {
	g.matrixstack[g.matrixindex ++] = g.transform;
}

void D_PopMatrix() {
	g.transform = g.matrixstack[-- g.matrixindex];
}




static void ApplyTransform(f32 x, f32 y, R_Vertex3 *vertices, int num) {
	for (int i = 0; i < num; i ++) {
		vec4 position = { vertices[i].position.x, vertices[i].position.y, vertices[i].position.z, 1.0 };
		position = MultiplyMatrixVector(g.transform, position);
		vertices[i].position.x = position.x + x;
		vertices[i].position.y = position.y + y;
		vertices[i].position.z = position.z;

		//	vertices[i].position.x *= g.scale.x;
		//	vertices[i].position.y *= g.scale.y;
		//	vertices[i].position.x += g.offset.x + x;
		//	vertices[i].position.y += g.offset.y + y;
	}
}

static void EndDrawCall(f32 x, f32 y, R_Vertex3 *vertices, int num) {
	ApplyTransform(x, y, vertices, num);
	g.fliponce.x = 0;
	g.fliponce.y = 0;
}

void D_BeginQuads(R_Renderer *rend) {
	g.nquads = 0;
}

void D_PushQuad(R_Renderer *rend, Rect dst, iRect src) {
	g.quads[g.nquads].dst = dst;
	g.quads[g.nquads].src = src;
	g.nquads += 1;
}

void D_EndQuads(R_Renderer *rend) {
	// god-tier optimization
	if (g.nquads <= 0) return;

	R_SetTopology(rend, TOPO_TRIANGLES);

	R_Vertex3 *vertices = R_QueueVertices(rend, g.nquads * 6);
	R_Vertex3 *cursor = vertices;

	for (int i = 0; i < g.nquads; i ++) {
		iRect src = g.quads[i].src;
		Rect dst = g.quads[i].dst;

		vec3 r_p0 = {     0,     0, 0 };
		vec3 r_p1 = {     0, dst.h, 0 };
		vec3 r_p2 = { dst.w, dst.h, 0 };
		vec3 r_p3 = { dst.w,     0, 0 };

		f32 u0 = src.x * g.texture_inv_resolution.x;
		f32 v0 = src.y * g.texture_inv_resolution.y;
		f32 u1 = (src.x + src.w) * g.texture_inv_resolution.x;
		f32 v1 = (src.y + src.h) * g.texture_inv_resolution.y;

		cursor[0]=(R_Vertex3){r_p0,{u0,v1},g.color_0};
		cursor[1]=(R_Vertex3){r_p1,{u0,v0},g.color_1};
		cursor[2]=(R_Vertex3){r_p2,{u1,v0},g.color_2};
		cursor[3]=(R_Vertex3){r_p0,{u0,v1},g.color_0};
		cursor[4]=(R_Vertex3){r_p2,{u1,v0},g.color_2};
		cursor[5]=(R_Vertex3){r_p3,{u1,v1},g.color_3};
		ApplyTransform(dst.x, dst.y, cursor, 6);
		cursor += 6;
	}

}


int D_GetFont() { return g.font; }
void D_SetFont(int font) { g.font = font; }

void D_Clear(R_Renderer *rend, Color color) {
	R_ClearSurface(rend, color);
}

void D_SolidFill(R_Renderer *rend) {
	D_SetTexture(rend, TEXTURE_DEFAULT);
}

Color D_GetColor0() { return g.color_0; }
Color D_GetColor1() { return g.color_1; }
Color D_GetColor2() { return g.color_2; }
Color D_GetColor3() { return g.color_3; }

// todo: cap these to 255!
void D_SetColor0(Color color) { g.color_0 = color; }
void D_SetColor1(Color color) { g.color_1 = color; }
void D_SetColor2(Color color) { g.color_2 = color; }
void D_SetColor3(Color color) { g.color_3 = color; }

void D_SetColor(Color color) {
	g.color_0 = g.color_1 = g.color_2 = g.color_3 = color;
}

void D_SetAlpha(int a) {
	g.color_0.a = g.color_1.a = g.color_2.a = g.color_3.a = a;
}

void D_SetScale(f32 x, f32 y) {
	g.transform = MultiplyMatrices(g.transform, ScaleMatrix((vec3){ x, y, 1 }));
}

vec3 D_GetScale() {
	return (vec3) { g.transform.rows[0].x, g.transform.rows[1].y, g.transform.rows[2].z };
}

// no need for a full matrix multiply right...
void D_Translate(f32 x, f32 y) {
	g.transform.rows[0].w += x;
	g.transform.rows[1].w += y;
}


void D_SetOffset(f32 x, f32 y) {
	g.offset.x = x;
	g.offset.y = y;
}

void D_SetCenter(f32 x, f32 y) {
	g.center.x = x;
	g.center.y = y;
}

void D_SetFlipOnce(int x, int y) {
	g.fliponce.x = x;
	g.fliponce.y = y;
}

void D_SetTexture(R_Renderer *rend, TextureId id) {
	R_SetTexture(rend, id);

	vec2i resolution = R_GetTextureInfo(rend, id);

	g.texture_inv_resolution.x = 1.0 / resolution.x;
	g.texture_inv_resolution.y = 1.0 / resolution.y;

	g.region.u0 = 0;
	g.region.v0 = 0;
	g.region.u1 = 1;
	g.region.v1 = 1;
}

void D_SetRegion(i32 x0, i32 y0, i32 x1, i32 y1) {
	g.region.u0 = x0 * g.texture_inv_resolution.x;
	g.region.v0 = y0 * g.texture_inv_resolution.y;
	g.region.u1 = x1 * g.texture_inv_resolution.x;
	g.region.v1 = y1 * g.texture_inv_resolution.y;
}


void D_SetRotation(f32 radians) {
	// todo: set transform directly here
	g.transform = MultiplyMatrices(g.transform, RotationMatrix(radians));
}

void D_DrawRectangle(R_Renderer *rend, f32 x, f32 y, f32 w, f32 h) {
	R_SetTopology(rend, TOPO_TRIANGLES);

	f32 u0 = g.region.u0;
	f32 v0 = g.region.v0;
	f32 u1 = g.region.u1;
	f32 v1 = g.region.v1;

	f32 temp;
	if (g.fliponce.x) { temp = u0; u0 = u1, u1 = temp; }
	if (g.fliponce.y) { temp = v0; v0 = v1, v1 = temp; }

	R_Vertex3 *vertices = R_QueueVertices(rend, 6);
	vertices[0]=(R_Vertex3){{ 0, 0 },{ u0, v1 }, g.color_0 };
	vertices[1]=(R_Vertex3){{ 0, h },{ u0, v0 }, g.color_1 };
	vertices[2]=(R_Vertex3){{ w, h },{ u1, v0 }, g.color_2 };
	vertices[3]=(R_Vertex3){{ 0, 0 },{ u0, v1 }, g.color_0 };
	vertices[4]=(R_Vertex3){{ w, h },{ u1, v0 }, g.color_2 };
	vertices[5]=(R_Vertex3){{ w, 0 },{ u1, v1 }, g.color_3 };
	EndDrawCall(x, y, vertices, 6);
}

// todo: circle quality should be a brush param, who the
// heck wants to pass this in each time!
void D_DrawCircle(R_Renderer *rend, f32 x, f32 y, f32 r, f32 v) {

	R_SetTopology(rend, TOPO_TRIANGLES);

	// todo: how do you actually make this proper...
	f32 circumference = TAU * r;
	i32 num_triangles = circumference / v;

	// todo: allow for inner color and outer color
	Color color = g.color_0;

	R_Vertex3 *vertices = R_QueueVertices(rend, num_triangles * 3);

	for (i32 i = 0; i < num_triangles; i ++) {
		i32 t = i * 3;
		vertices[t + 0] = (R_Vertex3){{x,y},{0,0},color};

		f32 a = ((i + 0.0) / (f32) num_triangles) * TAU;
		f32 ax = x + cos(a) * r;
		f32 ay = y + sin(a) * r;
		vertices[t + 1] = (R_Vertex3){{ax,ay},{0,1},color};

		f32 b = ((i + 1.0) / (f32) num_triangles) * TAU;
		f32 bx = x + cos(b) * r;
		f32 by = y + sin(b) * r;
		vertices[t + 2] = (R_Vertex3){{bx,by},{1,1},color};
	}
}

void D_DrawLine(R_Renderer *rend, f32 x0, f32 y0, f32 x1, f32 y1) {

	R_SetTexture(rend, TEXTURE_DEFAULT);
	R_SetTopology(rend, MODE_LINES);

	R_Vertex3 *vertices = R_QueueVertices(rend, 2);
	vertices[0] = (R_Vertex3){{x0,y0},{0,0},g.color_0};
	vertices[1] = (R_Vertex3){{x1,y1},{1,1},g.color_1};
}

void D_DrawTriangle(R_Renderer *rend, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2) {
	R_SetTopology(rend, TOPO_TRIANGLES);

	R_Vertex3 *vertices = R_QueueVertices(rend, 3);
	vertices[0] = (R_Vertex3){{x0,y0},{0,0},g.color_0};
	vertices[1] = (R_Vertex3){{x1,y1},{1,1},g.color_1};
	vertices[2] = (R_Vertex3){{x2,y2},{1,1},g.color_2};
}

