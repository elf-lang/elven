

D_DrawState gd;

/*
	InitWindow(title, base_reso_x ?= 0, base_reso_y ?= 0, scale ?= 0)
		Creates a window and initializes all the state
		needed to begin graphical operations.

		Use 'base_reso_x' and 'base_reso_y' if you're targeting
		a fixed base resolution.

		This simply results in the system automating some work
		for you.

		Many pixel art games choose to work with a fixed output
		resolution, for instance 320x180 or some other 16:9 ratio.
		This is called the 'base' resolution.

		Given the mismatch between the base resolution and the
		window's resolution, one must compute an 'output' rectangle.

		A rectangle that is the biggest multiple of the base
		resolution that still fits within the window is used,
		this rectangle is then centered within the window.

		Additionally, mouse coordinates are mapped within
		this output rectangle.


	GetScreenW()
	GetScreenH()
		Retrieve the width and height of the window.


	Button(key)
		Retrieve the button state for a given key.
		The result is a button state integer, where
		1 means down, 2 means pressed, 4 means released
		and 8 means repeat.


	MouseButton(index)
		Retrieve the button state for a given mouse
		button, this is the same as calling Button
		with an offset.


	GetMouseWheel()
		Retrieves a integer indicating the direction
		the mouse wheel is scrolling, 0 if no scroll.


	GetMouseX()
	GetMouseY()
		Retrieve mouse x and y coordinates within the output
		rectangle.
		** the result are normalized coordinates **



	PollWindow()
		Polls the window for event, and sleeps for the
		required amount of time.


	SetLayerColor(layer, 	r, g, b, a ?= 255)
		Sets the color register at index 'layer'

	SetLayerAlpha(layer, 	a)
		Sets the alpha of the color register at index 'layer'

	SetColor(r, g, b, a ?= 255)
		Sets all the color registers to the specified color

	@deprecated
	SetColorP(rgba)
		Same as set color, but the color is packed
		into a single integer.
		R G B A, where R is towards the MSB and A towards the LSB

	SetAlpha(a)
		Sets the alpha of all color registers.


	SetColor0(r, g, b, a ?= 255)
		Sets color register 0

	SetColor1(r, g, b, a ?= 255)
		Sets color register 1

	SetColor2(r, g, b, a ?= 255)
		Sets color register 2

	SetColor3(r, g, b, a ?= 255)
		Sets color register 3


	SetOutput(rid)
		Binds the specified output texture.
		The viewport and virtual resolution are set to match the
		output buffer's resolution.
		An output must be bound before any draw call.


	SetOutputWindow()
		Binds the windows's output texture.
		The window's output texture is bound on a call
		to BeginDrawing.


	Clear()
		Clears the currently bound output.

	SetViewport(w, h)
		Binds the viewport size.

	SetVirtualRes(w, h)
		Binds the virtual resolution.

	SetShader(shader_id)
		Binds the pixel shader.


	SetTexture(rid)
		Binds an input texture.
		This will set the current 'region' to match
		the texture's resolution.


	SolidFill()
		If the current texture does not support
		solid fill, it binds the default solid
		fill texture.


	SetRegion(x, y, w, h)
		Sets the current sampling region.
		** you must bind the desired texture before
		calling this function. **


	PushMatrix()
		Save a copy of the current matrix in the
		matrix stack.


	PopMatrix()
		Restores the transform matrix from the
		matrix stack.


	SetRotation(radians)
		Modifies the transform matrix directly to
		set its rotation.


	SetScale(x, y)
		Modifies the transform matrix directly to
		set its scale.


	SetOffset(x, y)
		Modifies the transform matrix directly to
		set its offset.


	Translate(x, y)
		Adds to the translation of the transform
		matrix.


	[[ color-0-1-2, texture, transform ]]
	DrawTriangle(x0, y0, x1, y1, x2, y2)
		Draws a single triangle.


	[[ color-0, transform ]]
	DrawLine(x0, y0, x1, y1)
		Draws a single line.


	[[ color-0, transform ]]
	DrawCircle(x, y, r, v ?= 1.0)


	[[ color-0-1-2-3, transform ]]
	DrawRectangle(x, y, w, h)


	[[ color-0, transform ]]
	DrawRectOutline(x, y, z)


*/

// L_GetTranslation
// L_SetFlipOnce
//
// L_GetTextureInfo
// L_LoadTexture
//
// L_NewOutputTexture
// L_BlitToWindow
// L_EndDrawing
// L_BeginDrawing
//
// L_SetFont
// L_LoadFont
// L_MeasureText
// L_DrawText
// L_GetFileDrop
// L_GetNumFileDrops
// L_OpenFileDialog
// L_GetNumVoices
// L_GetVoiceSound
// L_InitAudio
// L_LoadSound
// L_PlaySound
// L_StopVoice





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


void D_Clear(Color color) {
	R_ClearSurface(gd.rend, color);
}

void D_SolidFill() {
	D_SetTexture(gd.default_texture);
}

Color D_GetColor() { return gd.color_0; }

Color D_GetColor0() { return gd.color_0; }
Color D_GetColor1() { return gd.color_1; }
Color D_GetColor2() { return gd.color_2; }
Color D_GetColor3() { return gd.color_3; }


void D_SetLayerColor(int layer, Color color) {
	gd.colors[layer] = color;
}

void D_SetLayerAlpha(int layer, int alpha) {
	gd.colors[layer].a = (u8) alpha;
}




vec3 D_GetScale() {
	return (vec3) { gd.transform.rows[0].x, gd.transform.rows[1].y, gd.transform.rows[2].z };
}



void D_SetScale(f32 x, f32 y) {
	// gd.transform = MultiplyMatrices(gd.transform, ScaleMatrix((vec3){ x, y, 1 }));
	gd.transform.rows[0].x = x;
	gd.transform.rows[1].y = y;
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
	D_SolidFill();
	R_SetTopology(gd.rend, MODE_LINES);

	R_Vertex3 *vertices = R_QueueVertices(gd.rend, 2);
	vertices[0] = (R_Vertex3){{x0,y0},{0,0},gd.color_0};
	vertices[1] = (R_Vertex3){{x1,y1},{1,1},gd.color_1};
	EndDrawCall(0, 0, vertices, 2);
}



void D_DrawTriangle(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2) {
	R_SetTopology(gd.rend, TOPO_TRIANGLES);

	R_Vertex3 *vertices = R_QueueVertices(gd.rend, 3);
	vertices[0] = (R_Vertex3){{x0,y0},{0,0},gd.color_0};
	vertices[1] = (R_Vertex3){{x1,y1},{1,1},gd.color_1};
	vertices[2] = (R_Vertex3){{x2,y2},{1,1},gd.color_2};
}



void D_SetColor(Color color) {
	gd.color_0 = gd.color_1 = color;
	gd.color_2 = gd.color_3 = color;
}



void D_SetAlpha(int alpha) {
	gd.color_0.a = gd.color_1.a = (u8) alpha;
	gd.color_2.a = gd.color_3.a = (u8) alpha;
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
	gd.color_0 = _get_color_arg(S, nargs);
	return 0;
}



ELF_FUNCTION(L_SetColor1) {
	gd.color_1 = _get_color_arg(S, nargs);
	return 0;
}



ELF_FUNCTION(L_SetColor2) {
	gd.color_2 = _get_color_arg(S, nargs);
	return 0;
}



ELF_FUNCTION(L_SetColor3) {
	gd.color_3 = _get_color_arg(S, nargs);
	return 0;
}



ELF_FUNCTION(L_SetViewport) {
	vec2i reso;
	reso.x = elf_loadint(S, 1);
	reso.y = elf_loadint(S, 2);
	R_SetViewport(gd.rend, reso);
	return 0;
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



ELF_FUNCTION(L_SetColorP) {
	unsigned int packed = elf_loadint(S, 1);
	Color color = (Color){ packed >> 24, packed >> 16, packed >> 8, packed >> 0 };
	D_SetColor(color);
	return 0;
}



ELF_FUNCTION(L_SolidFill) {
	D_SolidFill();
	return 0;
}



ELF_FUNCTION(L_SetTexture) {
	RID id = (RID) elf_loadsys(S, 1);
	D_SetTexture(id);
	return 0;
}



ELF_FUNCTION(L_SetShader) {
	ShaderId id = elf_loadint(S, 1);
	R_SetShader(gd.rend, id);
	return 0;
}



ELF_FUNCTION(L_SetRegion) {
	i32 x0 = elf_loadnum(S, 1);
	i32 y0 = elf_loadnum(S, 2);
	i32 x1 = x0 + elf_loadnum(S, 3);
	i32 y1 = y0 + elf_loadnum(S, 4);
	D_SetRegion(x0, y0, x1, y1);
	return 0;
}



ELF_FUNCTION(L_SetRotation) {
	D_SetRotation(elf_loadnum(S, 1));
	return 0;
}



ELF_FUNCTION(L_SetCenter) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_SetCenter(x, y);
	return 0;
}



ELF_FUNCTION(L_SetScale) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_SetScale(x, y);
	return 0;
}



ELF_FUNCTION(L_Translate) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_Translate(x, y);
	return 0;
}



ELF_FUNCTION(L_GetTranslation) {
	return 0;
}



ELF_FUNCTION(L_SetOffset) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_SetOffset(x, y);
	return 0;
}



ELF_FUNCTION(L_SetFlipOnce) {
	int x = elf_loadint(S, 1);
	int y = elf_loadint(S, 2);
	D_SetFlipOnce(x, y);
	return 0;
}



ELF_FUNCTION(L_PushMatrix) {
	D_PushMatrix();
	return 0;
}



ELF_FUNCTION(L_PopMatrix) {
	D_PopMatrix();
	return 0;
}



ELF_FUNCTION(L_Clear) {
	Color color = _get_color_arg(S, nargs);
	D_Clear(color);
	return 0;
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
	f32 v = elf_loadnum(S, 4);

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

	D_DrawRectangle(x, y-1, w, 1);
	D_DrawRectangle(x, y+h, w, 1);
	D_DrawRectangle(x-1, y, 1, h);
	D_DrawRectangle(x+w, y, 1, h);
	return 0;
}



ELF_FUNCTION(L_GetTextureInfo) {
	RID id = (RID) elf_loadsys(S, 1);
	vec2i reso = R_GetTextureInfo(gd.rend, id);

	elf_pushtab(S);

	elf_pushtext(S, "x");
	elf_pushint(S, reso.x);
	elf_setfield(S);

	elf_pushtext(S, "y");
	elf_pushint(S, reso.y);
	elf_setfield(S);
	return 1;
}



ELF_FUNCTION(L_UpdateTexture)
{
	RID     id = (RID)     elf_loadsys(S, 1);
	Image *img = (Image *) elf_loadsys(S, 2);
	int x      =           elf_loadint(S, 3);
	int y      =           elf_loadint(S, 4);
	int w      =           elf_loadint(S, 5);
	int h      =           elf_loadint(S, 6);
	R_UpdateTexture(gd.rend, id, (iRect){x,y,w,h}
	, img->data+x+y*img->reso.x
	, img->reso.x*sizeof(*img->data));
	return 0;
}



ELF_FUNCTION(L_LoadTexture)
{
	const char *name = elf_loadtext(S, 1);

	vec2i resolution;
	i32 num_channels;
	i32 wanted_channels = 4;

	unsigned char *colors = stbi_load(name, &resolution.x, &resolution.y, &num_channels, wanted_channels);

	if (colors) {
		RID rid = R_InstallTexture(gd.rend, FORMAT_R8G8B8_UNORM, resolution, colors);
		elf_pushsys(S, rid);

		free(colors);
	}
	else {
		elf_pushnil(S);
	}
	return 1;
}



// todo: possibly rename to "Keyboard" and then MouseButton to "Mouse"
ELF_FUNCTION(L_Button) {
	int state = 0;
	for (int i = 1; i < nargs; i ++) {
		i32 index = elf_loadint(S, i);
		state |= OS_GetWindowKey(gd.window, index);
	}
	elf_pushint(S, state);
	return 1;
}



ELF_FUNCTION(L_MouseButton) {
	i32 index = elf_loadint(S, 1);
	elf_pushint(S, OS_GetWindowKey(gd.window, KEY_MOUSE_LEFT + index));
	return 1;
}



ELF_FUNCTION(L_GetMouseWheel) {
	elf_pushint(S, OS_GetWindowMouseWheel(gd.window).y);
	return 1;
}



ELF_FUNCTION(L_GetMouseX) {
	elf_pushnum(S, gd.mouse.x);
	return 1;
}



ELF_FUNCTION(L_GetMouseY) {
	elf_pushnum(S, gd.mouse.y);
	return 1;
}



ELF_FUNCTION(L_SetVirtualRes) {
	vec2i reso;
	reso.x = elf_loadint(S, 1);
	reso.y = elf_loadint(S, 2);
	R_SetVirtualReso(gd.rend, reso);
	return 0;
}



ELF_FUNCTION(L_SetOutput) {

	RID rid = (RID) elf_loadsys(S, 1);
	R_SetOutput(gd.rend, rid);
	return 0;
}



ELF_FUNCTION(L_SetOutputWindow) {
	R_SetOutput(gd.rend, R_GetWindowOutput(gd.rend));
	return 0;
}



ELF_FUNCTION(L_NewOutputTexture) {
	vec2i reso;
	reso.x = elf_loadint(S, 1);
	reso.y = elf_loadint(S, 2);

	RID rid = R_InstallSurface(gd.rend, FORMAT_R8G8B8_UNORM, reso);
	elf_pushsys(S, rid);
	return 1;
}



static iRect GetOutputRectFor(vec2i dst_r, vec2i src_r) {
	i32 max_scale = MIN(dst_r.x / (f32) src_r.x, dst_r.y / (f32) src_r.y);

	iRect rect;
	rect.w = src_r.x * max_scale;
	rect.h = src_r.y * max_scale;
	rect.x = (dst_r.x - rect.w) * 0.5;
	rect.y = (dst_r.y - rect.h) * 0.5;
	return rect;
}



ELF_FUNCTION(L_GetScreenW) {
	elf_pushint(S, OS_GetWindowResolution(gd.window).x);
	return 1;
}



ELF_FUNCTION(L_GetScreenH) {
	elf_pushint(S, OS_GetWindowResolution(gd.window).y);
	return 1;
}



ELF_FUNCTION(L_BlitToWindow) {
	RID src = (RID) elf_loadsys(S, 1);
	RID dst = R_GetWindowOutput(gd.rend);


	vec2i dst_r = R_GetTextureInfo(gd.rend, dst);
	vec2i src_r = R_GetTextureInfo(gd.rend, src);
	iRect rect = GetOutputRectFor(dst_r, src_r);


	R_SetBlender(gd.rend, BLEND_DISABLE);
	R_SetTexture(gd.rend, src);
	R_SetOutput(gd.rend, dst);

	// R_ClearSurface(gd.rend, TextureFromRID(gd.rend, src)->clear_color);


	vec2i xy = rect.xy;
	vec2i wh = rect.wh;
	vec3 p0 = { xy.x + wh.x * 0, xy.y + wh.y * 0, 0 };
	vec3 p1 = { xy.x + wh.x * 0, xy.y + wh.y * 1, 0 };
	vec3 p2 = { xy.x + wh.x * 1, xy.y + wh.y * 0, 0 };
	vec3 p3 = { xy.x + wh.x * 1, xy.y + wh.y * 1, 0 };

	R_SetTopology(gd.rend, TOPO_TRIANGLES);
	R_Vertex3 *vertices = R_QueueVertices(gd.rend, 6);
	vertices[0] = (R_Vertex3) {p0, {0,1}, WHITE};
	vertices[1] = (R_Vertex3) {p1, {0,0}, WHITE};
	vertices[2] = (R_Vertex3) {p3, {1,0}, WHITE};
	vertices[3] = (R_Vertex3) {p0, {0,1}, WHITE};
	vertices[4] = (R_Vertex3) {p3, {1,0}, WHITE};
	vertices[5] = (R_Vertex3) {p2, {1,1}, WHITE};
	return 0;
}



ELF_FUNCTION(L_EndDrawing) {

	if (gd.base != RID_NONE) {
		iRect rect = gd.output_rect;

		R_SetBlender(gd.rend, BLEND_DISABLE);
		R_SetTexture(gd.rend, gd.base);
		R_SetOutput(gd.rend, R_GetWindowOutput(gd.rend));

		// todo: let the user specify the color of this
		if (rect.x != 0 || rect.y != 0) {
			R_ClearSurface(gd.rend, BLACK);
		}


		vec2i xy = rect.xy;
		vec2i wh = rect.wh;
		vec3 p0 = { xy.x + wh.x * 0, xy.y + wh.y * 0, 0 };
		vec3 p1 = { xy.x + wh.x * 0, xy.y + wh.y * 1, 0 };
		vec3 p2 = { xy.x + wh.x * 1, xy.y + wh.y * 0, 0 };
		vec3 p3 = { xy.x + wh.x * 1, xy.y + wh.y * 1, 0 };

		R_SetTopology(gd.rend, TOPO_TRIANGLES);
		R_Vertex3 *vertices = R_QueueVertices(gd.rend, 6);
		vertices[0] = (R_Vertex3) {p0, {0,1}, WHITE};
		vertices[1] = (R_Vertex3) {p1, {0,0}, WHITE};
		vertices[2] = (R_Vertex3) {p3, {1,0}, WHITE};
		vertices[3] = (R_Vertex3) {p0, {0,1}, WHITE};
		vertices[4] = (R_Vertex3) {p3, {1,0}, WHITE};
		vertices[5] = (R_Vertex3) {p2, {1,1}, WHITE};
	}


	R_EndFrame(gd.rend);
	return 0;
}



ELF_FUNCTION(L_BeginDrawing) {
	R_BeginFrame(gd.rend);

	// todo: load identity will do this
	D_SetScale(1, 1);
	D_SetOffset(0, 0);
	D_SetCenter(0, 0);

	D_LoadIdentity();
	D_SolidFill();

	R_SetOutput(gd.rend, gd.output_target);

	R_SetSampler(gd.rend, SAMPLER_POINT);
	R_SetTopology(gd.rend, TOPO_TRIANGLES);
	R_SetBlender(gd.rend, BLENDER_ALPHA_BLEND);
	return 0;
}



ELF_FUNCTION(L_InitWindow) {
	const char *name = elf_loadtext(S, 1);

	if (nargs >= 4) {
		gd.base_reso.x = elf_loadint(S, 2);
		gd.base_reso.y = elf_loadint(S, 3);
	}

	gd.window = OS_InstallWindow(name, gd.base_reso);

	gd.rend = R_InitRenderer(gd.window);
	gd.output_target = R_GetWindowOutput(gd.rend);

	if (gd.base_reso.x != 0 && gd.base_reso.y != 0) {
		gd.base = R_InstallSurface(gd.rend, FORMAT_R8G8B8_UNORM, gd.base_reso);
		gd.output_target = gd.base;
	}

	Color color = {255, 255, 255, 255};
	gd.default_texture = R_InstallTexture(gd.rend, FORMAT_R8G8B8_UNORM, (vec2i){1,1}, &color);

	gd.target_seconds_to_sleep = 1.0 / 60.0;
	gd.begin_cycle_clock = OS_GetTickCounter();
	return 0;
}



ELF_FUNCTION(L_PollWindow) {

	b32 open = OS_PollWindow(gd.window);
	if (!open) goto esc;

	vec2i window_r = OS_GetWindowResolution(gd.window);

	gd.output_rect.x = 0;
	gd.output_rect.y = 0;
	gd.output_rect.wh = window_r;

	if (gd.base != RID_NONE) {
		gd.output_rect = GetOutputRectFor(window_r, gd.base_reso);
	}

	vec2i mouse = OS_GetWindowMouse(gd.window);
	gd.mouse.x = 0 + (mouse.x - gd.output_rect.x) / (f32) gd.output_rect.w;
	gd.mouse.y = 1 - (mouse.y - gd.output_rect.y) / (f32) gd.output_rect.h;


	// because we only have one window
	OS_ForgetFileDrops();


	// todo: fix this thing, it works too well
	i64 end_cycle_clock = OS_GetTickCounter();
	i64 elapsed_clocks = end_cycle_clock - gd.begin_cycle_clock;
	f64 elapsed_seconds = elapsed_clocks * OS_GetClocksToSeconds();
	gd.pending_seconds_to_sleep += gd.target_seconds_to_sleep - elapsed_seconds;

	// todo: instead of making this logic work in seconds
	// make it work in clock cycles directly
	f64 clock_accuracy = 1.0 / 1000.0;
	gd.begin_cycle_clock = OS_GetTickCounter();
	while (gd.pending_seconds_to_sleep > clock_accuracy) {
		OS_Sleep(gd.pending_seconds_to_sleep * 1000);
		gd.begin_cycle_clock = OS_GetTickCounter();
		gd.pending_seconds_to_sleep -= (gd.begin_cycle_clock - end_cycle_clock) * OS_GetClocksToSeconds();
	}

	esc:
	elf_pushint(S, open);
	return 1;
}



ELF_FUNCTION(L_GetFileDrop) {

	WID window = gd.window;
	i32 index = elf_loadint(S, 1);
	elf_pushtext(S, OS_GetFileDrop(index));
	return 1;
}



ELF_FUNCTION(L_GetNumFileDrops) {

	WID window = gd.window;
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






elf_Binding l_state[] = {
	{ "InitWindow"                       , L_InitWindow                           },
	{ "PollWindow"                       , L_PollWindow                           },

	{ "GetScreenW"                       , L_GetScreenW                           },
	{ "GetScreenH"                       , L_GetScreenH                           },

	{ "BlitToWindow"                     , L_BlitToWindow                         },

	{ "NewOutputTexture"                 , L_NewOutputTexture                     },
	{ "SetVirtualRes"                    , L_SetVirtualRes                        },
	{ "SetOutput"                        , L_SetOutput                            },
	{ "SetOutputWindow"                  , L_SetOutputWindow                      },

	{ "UpdateTexture"                    , L_UpdateTexture                        },
	{ "LoadTexture"                      , L_LoadTexture                          },
	{ "GetTextureInfo"                   , L_GetTextureInfo                       },

	{ "PushMatrix"                       , L_PushMatrix                           },
	{ "PopMatrix"                        , L_PopMatrix                            },
	{ "SetScale"                         , L_SetScale                             },
	{ "SetOffset"                        , L_SetOffset                            },
	{ "SetRotation"                      , L_SetRotation                          },
	{ "SetCenter"                        , L_SetCenter                            },
	{ "Translate"                        , L_Translate                            },
	{ "GetTranslation"                   , L_GetTranslation                       },


	{ "Button"                           , L_Button                               },
	{ "MouseButton"                      , L_MouseButton                          },
	{ "GetMouseWheel"                    , L_GetMouseWheel                        },
	{ "GetMouseX"                        , L_GetMouseX                            },
	{ "GetMouseY"                        , L_GetMouseY                            },

	{ "SetViewport"                      , L_SetViewport                          },
	{ "SetTexture"                       , L_SetTexture                           },
	{ "SetShader"                        , L_SetShader                            },

	{ "Clear"                            , L_Clear                                },
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

	{ "BeginDrawing"                     , L_BeginDrawing                         },
	{ "EndDrawing"                       , L_EndDrawing                           },
	{ "DrawRectangle"                    , L_DrawRectangle                        },
	{ "DrawRectOutline"                  , L_DrawRectOutline                      },
	{ "DrawTriangle"                     , L_DrawTriangle                         },
	{ "DrawLine"                         , L_DrawLine                             },
	{ "DrawCircle"                       , L_DrawCircle                           },
	{ "SetFlipOnce"                      , L_SetFlipOnce                          },

	{ "GetFileDrop"                      , L_GetFileDrop                          },
	{ "GetNumFileDrops"                  , L_GetNumFileDrops                      },
	{ "OpenFileDialog"                   , L_OpenFileDialog                       },
};
