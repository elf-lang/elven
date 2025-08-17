

void D_PushMatrix() {
	gd.matrixstack[gd.matrixindex ++] = gd.transform;
}



void D_PopMatrix() {
	gd.transform = gd.matrixstack[-- gd.matrixindex];
}



static Matrix ScaleMatrix(vec3 v) {
	Matrix c = {
		v.x,   0,   0, 0,
		0, v.y,   0, 0,
		0,   0, v.z, 0,
		0,   0,   0, 1,
	};
	return c;
}



static Matrix RotationMatrix(f32 r) {
	f32 co = cosf(r);
	f32 si = sinf(r);
	Matrix c = {
		co,  si, 0, 0,
		si, -co, 0, 0,
		0,    0, 1, 0,
		0,    0, 0, 1,
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



static vec4 MultiplyMatrixVector(Matrix m, vec4 v) {
	vec4 c;
	for (int i = 0; i < 4; i ++) {
		c.xyzw[i] =
		v.xyzw[0] * m.rows[i].xyzw[0] +
		v.xyzw[1] * m.rows[i].xyzw[1] +
		v.xyzw[2] * m.rows[i].xyzw[2] +
		v.xyzw[3] * m.rows[i].xyzw[3];
	}
	return c;
}



void D_LoadIdentity() {
	Matrix c = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	gd.transform = c;
}



// todo: this is so weird!
static void ApplyTransform(f32 x, f32 y, R_Vertex3 *vertices, int num) {
	for (int i = 0; i < num; i ++) {
		vec4 position = { vertices[i].position.x, vertices[i].position.y, vertices[i].position.z, 1.0 };
		position = MultiplyMatrixVector(gd.transform, position);
		vertices[i].position.x = position.x + x;
		vertices[i].position.y = position.y + y;
		vertices[i].position.z = position.z;
	}
}

static void EndDrawCall(f32 x, f32 y, R_Vertex3 *vertices, int num) {
	ApplyTransform(x, y, vertices, num);
	gd.mirrortextureonce.x = 0;
	gd.mirrortextureonce.y = 0;
}


void D_BeginQuads() {
	gd.nquads = 0;
}

void D_PushQuad(Rect dst, iRect src) {
	ASSERT(gd.nquads < COUNTOF(gd.quads));
	gd.quads[gd.nquads].dst = dst;
	gd.quads[gd.nquads].src = src;
	gd.nquads += 1;
}


void D_EndQuads() {
	// god-tier optimization
	if (gd.nquads <= 0) return;

	R_SetTopology(gd.rend, TOPO_TRIANGLES);

	R_Vertex3 *vertices = R_QueueVertices(gd.rend, gd.nquads * 6);
	R_Vertex3 *cursor = vertices;

	for (int i = 0; i < gd.nquads; i ++) {
		iRect src = gd.quads[i].src;
		Rect dst = gd.quads[i].dst;

		vec3 r_p0 = {     0,     0, 0 };
		vec3 r_p1 = {     0, dst.h, 0 };
		vec3 r_p2 = { dst.w, dst.h, 0 };
		vec3 r_p3 = { dst.w,     0, 0 };

		f32 u0 = src.x * gd.texture_inv_resolution.x;
		f32 v0 = src.y * gd.texture_inv_resolution.y;
		f32 u1 = (src.x + src.w) * gd.texture_inv_resolution.x;
		f32 v1 = (src.y + src.h) * gd.texture_inv_resolution.y;

		cursor[0]=(R_Vertex3){r_p0,{u0,v1},gd.color_0};
		cursor[1]=(R_Vertex3){r_p1,{u0,v0},gd.color_1};
		cursor[2]=(R_Vertex3){r_p2,{u1,v0},gd.color_2};
		cursor[3]=(R_Vertex3){r_p0,{u0,v1},gd.color_0};
		cursor[4]=(R_Vertex3){r_p2,{u1,v0},gd.color_2};
		cursor[5]=(R_Vertex3){r_p3,{u1,v1},gd.color_3};
		// todo:
		ApplyTransform(dst.x, dst.y, cursor, 6);
		cursor += 6;
	}

}


FONT_HANDLE D_GetFont() { return gd.font; }
void D_SetFont(FONT_HANDLE  font) { gd.font = font; }


void D_Clear(Color color) {
	R_ClearSurface(gd.rend, color);
}

void D_SolidFill() {
	D_SetTexture(RID_TEXTURE_DEFAULT);
}

Color D_GetColor() { return gd.color_0; }

Color D_GetColor0() { return gd.color_0; }
Color D_GetColor1() { return gd.color_1; }
Color D_GetColor2() { return gd.color_2; }
Color D_GetColor3() { return gd.color_3; }

void D_SetColor0(Color color) { gd.color_0 = color; }
void D_SetColor1(Color color) { gd.color_1 = color; }
void D_SetColor2(Color color) { gd.color_2 = color; }
void D_SetColor3(Color color) { gd.color_3 = color; }


void D_SetLayerColor(int layer, Color color) {
	gd.colors[layer] = color;
}

void D_SetLayerAlpha(int layer, int alpha) {
	gd.colors[layer].a = (u8) alpha;
}

void D_SetColor(Color color) {
	gd.color_0 = gd.color_1 = gd.color_2 = gd.color_3 = color;
}
void D_SetAlpha(int alpha) {
	gd.color_0.a = gd.color_1.a = gd.color_2.a = gd.color_3.a = (u8) alpha;
}



void D_SetScale(f32 x, f32 y) {
	gd.transform = MultiplyMatrices(gd.transform, ScaleMatrix((vec3){ x, y, 1 }));
}

vec3 D_GetScale() {
	return (vec3) { gd.transform.rows[0].x, gd.transform.rows[1].y, gd.transform.rows[2].z };
}

void D_Translate(f32 x, f32 y) {
	gd.transform.rows[0].w += x;
	gd.transform.rows[1].w += y;
}


void D_SetOffset(f32 x, f32 y) {
	gd.transform.rows[0].w = x;
	gd.transform.rows[1].w = y;
}



void D_SetCenter(f32 x, f32 y) {
}



void D_SetFlipOnce(int x, int y) {
	gd.mirrortextureonce.x = x;
	gd.mirrortextureonce.y = y;
}



void D_SetTexture(RID id) {
	R_SetTexture(gd.rend, id);

	vec2i resolution = R_GetTextureInfo(gd.rend, id);

	gd.texture_inv_resolution.x = 1.0 / resolution.x;
	gd.texture_inv_resolution.y = 1.0 / resolution.y;

	gd.region.u0 = 0;
	gd.region.v0 = 0;
	gd.region.u1 = 1;
	gd.region.v1 = 1;
}

void D_SetRegion(i32 x0, i32 y0, i32 x1, i32 y1) {
	gd.region.u0 = x0 * gd.texture_inv_resolution.x;
	gd.region.v0 = y0 * gd.texture_inv_resolution.y;
	gd.region.u1 = x1 * gd.texture_inv_resolution.x;
	gd.region.v1 = y1 * gd.texture_inv_resolution.y;
}


void D_SetRotation(f32 radians) {
	// todo: set transform directly here
	gd.transform = MultiplyMatrices(gd.transform, RotationMatrix(radians));
}

void D_DrawRectangle(f32 x, f32 y, f32 w, f32 h) {
	R_SetTopology(gd.rend, TOPO_TRIANGLES);

	f32 u0 = gd.region.u0;
	f32 v0 = gd.region.v0;
	f32 u1 = gd.region.u1;
	f32 v1 = gd.region.v1;

	if (w == 0) w = R_GetTextureInfo(gd.rend, R_GetTexture(gd.rend)).x;
	if (h == 0) h = R_GetTextureInfo(gd.rend, R_GetTexture(gd.rend)).y;

	f32 temp;
	if (gd.mirrortextureonce.x) { temp = u0; u0 = u1, u1 = temp; }
	if (gd.mirrortextureonce.y) { temp = v0; v0 = v1, v1 = temp; }

	R_Vertex3 *vertices = R_QueueVertices(gd.rend, 6);
	vertices[0]=(R_Vertex3){{ 0, 0 },{ u0, v1 }, gd.color_0 };
	vertices[1]=(R_Vertex3){{ 0, h },{ u0, v0 }, gd.color_1 };
	vertices[2]=(R_Vertex3){{ w, h },{ u1, v0 }, gd.color_2 };
	vertices[3]=(R_Vertex3){{ 0, 0 },{ u0, v1 }, gd.color_0 };
	vertices[4]=(R_Vertex3){{ w, h },{ u1, v0 }, gd.color_2 };
	vertices[5]=(R_Vertex3){{ w, 0 },{ u1, v1 }, gd.color_3 };
	EndDrawCall(x, y, vertices, 6);
}

// todo: circle quality should be a brush param, who the
// heck wants to pass this in each time!
void D_DrawCircle(f32 x, f32 y, f32 r, f32 v) {
	R_Renderer *rend = gd.rend;

	R_SetTopology(rend, TOPO_TRIANGLES);

	// todo: how do you actually make this proper...
	f32 circumference = TAU * r;
	i32 num_triangles = circumference / v;

	// todo: allow for inner color and outer color
	Color color = gd.color_0;

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



void D_DrawLine(f32 x0, f32 y0, f32 x1, f32 y1) {
	D_SetTexture(RID_TEXTURE_DEFAULT);
	R_SetTopology(gd.rend, MODE_LINES);

	R_Vertex3 *vertices = R_QueueVertices(gd.rend, 2);
	vertices[0] = (R_Vertex3){{x0,y0},{0,0},gd.color_0};
	vertices[1] = (R_Vertex3){{x1,y1},{1,1},gd.color_1};
	EndDrawCall(0, 0, vertices, 2);
}



void D_DrawTriangle(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2) {
	R_Renderer *rend = gd.rend;
	R_SetTopology(rend, TOPO_TRIANGLES);

	R_Vertex3 *vertices = R_QueueVertices(rend, 3);
	vertices[0] = (R_Vertex3){{x0,y0},{0,0},gd.color_0};
	vertices[1] = (R_Vertex3){{x1,y1},{1,1},gd.color_1};
	vertices[2] = (R_Vertex3){{x2,y2},{1,1},gd.color_2};
}

