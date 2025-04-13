#define _DEBUG
#include "elf.c"

// <3
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define _CRT_SECURE_NO_WARNINGS
#define              CINTERFACE
#define              COBJMACROS
#define        D3D11_NO_HELPERS
#pragma comment(lib,        "Gdi32")
#pragma comment(lib,       "dxguid")
#pragma comment(lib,        "d3d11")
#pragma comment(lib,  "d3dcompiler")
#include <d3dcompiler.h>
#include   <dxgidebug.h>
#include        <dxgi.h>
#include       <d3d11.h>
#include     <dxgi1_3.h>

#include "jam.h"
#include "os_win32.c"
#include "d3d11_renderer.c"
#include "fonts.c"

Vertex2D     *r_mem_vertices;
i32           r_num_vertices;
i32           r_max_vertices;

vec2      	  r_scale;
vec2      	  r_offset;
f32       	  r_rotation;
vec2      	  r_center;
Color         r_color;


static void rDrawVertices(JState *J, rVertexOffsetInSubmissionBuffer offset, i32 num_vertices);
static void rFlushVertices(JState *J);


static void rInstallShader(JState *J, ShaderId id, char *entry, char *contents) {

	enum {
		SHADER_COMPILE_FLAGS = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR|D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION|D3DCOMPILE_WARNINGS_ARE_ERRORS,
	};

	ID3DBlob *bytecode_b = NULL;
	ID3DBlob *messages_b = NULL;

	ID3D11PixelShader *shader = NULL;

	if (SUCCEEDED(D3DCompile(contents, strlen(contents)
	,		entry, 0, 0, entry, "ps_5_0"
	,		SHADER_COMPILE_FLAGS, 0, &bytecode_b, &messages_b)))
	{
		ID3D11Device_CreatePixelShader(J->device
		, bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
		, bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
		, NULL, &shader);
	}
	if (messages_b) {
		TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
	}
	if (bytecode_b) bytecode_b->lpVtbl->Release(bytecode_b);
	if (messages_b) messages_b->lpVtbl->Release(messages_b);

	J->shaders[id.index] = shader;
}

static inline void rSetShader(JState *J, ShaderId shader) {
	if (J->draw_prox.shader.index != shader.index) {
		rFlushVertices(J);
		J->draw_prox.shader = shader;
	}
}

static inline void rSetTopology(JState *J, i32 topology) {
	if (J->draw_prox.topology != topology) {
		rFlushVertices(J);
		J->draw_prox.topology = topology;
	}
}

static inline void rSetTexture(JState *J, TextureId id) {
	if (J->draw_prox.texture != id) {
		rFlushVertices(J);
		J->draw_prox.texture = id;
	}
}

static inline void rFlushVertices(JState *J) {
	if (r_num_vertices != 0) {
		rVertexOffsetInSubmissionBuffer offset = rSubmitVertices(J, r_mem_vertices, r_num_vertices);
		rDrawVertices(J, offset, r_num_vertices);
		r_num_vertices = 0;
	}
}

static void rDrawVertices(JState *J, rVertexOffsetInSubmissionBuffer offset, i32 number) {

	Draw_State prev = J->draw_prev;
	Draw_State prox = J->draw_prox;
	J->draw_prev = prox;

	ASSERT(number != 0);

	switch (prox.topology) {
		case MODE_TRIANGLES: { ASSERT(number % 3 == 0); } break;
		case MODE_LINES:     { ASSERT(number % 2 == 0); } break;
		default: { ASSERT(!"SHMUCK"); } break;
	}


	ASSERT(prox.topology != MODE_NONE);
	ASSERT(prox.texture != TEXTURE_NONE);


	if (prev.topology != prox.topology) {
		ID3D11DeviceContext_IASetPrimitiveTopology(J->context, prox.topology);
	}

	if (prev.shader.index != prox.shader.index) {
		ID3D11PixelShader *shader = J->shaders[prox.shader.index];
		ID3D11DeviceContext_PSSetShader(J->context, shader, 0, 0);
	}

	if (prev.texture != prox.texture) {
		rTextureStruct *texture = & J->textures[prox.texture];
		ID3D11ShaderResourceView *shader_resource_view = texture->shader_resource_view;
		ID3D11SamplerState *sampler = texture->sampler;
		ID3D11DeviceContext_PSSetShaderResources(J->context, 0, 1, &shader_resource_view);
		ID3D11DeviceContext_PSSetSamplers(J->context, 0, 1, &sampler);
	}

	ID3D11DeviceContext_Draw(J->context, number, offset.offset);
}

static inline Vertex2D *rReserveVertices(JState *jam, i32 num) {
	if (r_num_vertices + num > r_max_vertices) {
		rFlushVertices(jam);
	}
	Vertex2D *vertices = r_mem_vertices + r_num_vertices;
	r_num_vertices += num;
	return vertices;
}

static int L_Button(elf_State *S) {
	i32 key = elf_get_int(S,0);
	elf_add_int(S, os.keyboard[key].u);
	return 1;
}

static int L_GetCursorX(elf_State *S) {
	JState *J = (JState *) S;
	elf_add_num(S, os.mouse_xy.x / (f32) J->window_dimensions.x);
	return 1;
}

static int L_GetCursorY(elf_State *S) {
	JState *J = (JState *) S;
	elf_add_num(S, 1 - os.mouse_xy.y / (f32) J->window_dimensions.y);
	return 1;
}

static int L_InitWindow(elf_State *S) {
	JState *J = (JState *) S;

	int i = 0;
	char *name = elf_get_text(S,i++);
	int resolution_x = elf_get_int(S,i++);
	int resolution_y = elf_get_int(S,i++);
	int window_scale = elf_get_int(S,i++);

	if (window_scale <= 0) {
		window_scale = 1;
	}

	HWND window = init_os(name, (vec2i){resolution_x, resolution_y}, window_scale);

	rInitRenderer((Init_Renderer) {
		.jam = J,
		.window = window,
		.res_x = resolution_x,
		.res_y = resolution_y
	});


#if defined(_WIN32)
	r_mem_vertices = VirtualAlloc(0, MEGABYTES(128), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	r_max_vertices = MEGABYTES(128) / sizeof(*r_mem_vertices);
	r_num_vertices = 0;
	r_scale.x = 1;
	r_scale.y = 1;
	rSetTexture(J, TEXTURE_DEFAULT);
#endif

	J->target_seconds_to_sleep = 1.0 / 60.0;
	J->begin_cycle_clock = os_clock();
	return 0;
}


static int L_PollWindow(elf_State *S) {
	JState *J = (JState *) S;

#if defined(_WIN32)

	for (i32 i = 0; i < COUNTOF(os.keyboard); i ++) {
		os.keyboard[i].u &= DOWN_BIT;
	}

	{
		MSG msg = {};
		while(PeekMessage(&msg, os.window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		RECT window_r;
		GetClientRect(os.window, &window_r);
		J->window_dimensions.x = window_r.right - window_r.left;
		J->window_dimensions.y = window_r.bottom - window_r.top;
	}
#endif

	rFlushVertices(J);

	EndFrame(J);

	// todo: this happens because the renderer
	// internally changes the texture
	J->draw_prev.texture  = 0;
	J->draw_prev.topology = 0;

	// todo: this seems to work pretty good,
	// but we have to actually do it properly...
	S->G.paused = false;
	_gc_check(S,0);
	S->G.paused = true;

	// todo:
	i64 end_cycle_clock = os_clock();
	i64 elapsed_clocks = end_cycle_clock - J->begin_cycle_clock;
	f64 elapsed_seconds = elapsed_clocks * os.clocks_to_seconds;
	J->pending_seconds_to_sleep += J->target_seconds_to_sleep - elapsed_seconds;


	f64 clock_accuracy = 1.0 / 1000.0;
	J->begin_cycle_clock = os_clock();
	while (J->pending_seconds_to_sleep > clock_accuracy) {
		Sleep(J->pending_seconds_to_sleep * 1000);
		J->begin_cycle_clock = os_clock();
		J->pending_seconds_to_sleep -= (J->begin_cycle_clock - end_cycle_clock) * os.clocks_to_seconds;
	}

	elf_add_int(S,!os.close_window);
	return 1;
}

int L_SetColor(elf_State *S) {
	if (elf_get_num_args(S) > 2) {
		r_color.r = elf_get_int(S, 0);
		r_color.g = elf_get_int(S, 1);
		r_color.b = elf_get_int(S, 2);
		if (elf_get_num_args(S) > 3) {
			r_color.a = elf_get_int(S, 3);
		}
	}
	return 0;
}

int L_SetRotation(elf_State *S) {
	r_rotation = elf_get_num(S,0);
	return 0;
}

int L_SetCenter(elf_State *S) {
	r_center.x = elf_get_num(S,0);
	r_center.y = elf_get_num(S,1);
	return 0;
}

int L_SetScale(elf_State *S) {
	r_scale.x = elf_get_num(S,0);
	r_scale.y = elf_get_num(S,1);
	return 0;
}

int L_SetOffset(elf_State *S) {
	r_offset.x = elf_get_num(S,0);
	r_offset.y = elf_get_num(S,1);
	return 0;
}

static int L_Clear(elf_State *S) {
	JState *J = (JState *) S;

	Color color = {0, 0, 0, 0};

	if (elf_get_num_args(S) > 2) {
		color.r = elf_get_int(S, 0);
		color.g = elf_get_int(S, 1);
		color.b = elf_get_int(S, 2);
		if (elf_get_num_args(S) > 3) {
			color.a = elf_get_int(S, 3);
		}
	}

	f32 inv = 1.0 / 255.0;

	ID3D11RenderTargetView *render_target_view = J->base_render_target_view;
	f32 fcolor[4] = { color.r * inv, color.g * inv, color.b * inv, color.a * inv };
	ID3D11DeviceContext_ClearRenderTargetView(J->context, render_target_view, fcolor);
	return 0;
}

static int L_DrawTriangle(elf_State *S) {
	JState *J = (JState *) S;

	f32 x0 = elf_get_num(S, 0);
	f32 y0 = elf_get_num(S, 1);

	f32 x1 = elf_get_num(S, 2);
	f32 y1 = elf_get_num(S, 3);

	f32 x2 = elf_get_num(S, 4);
	f32 y2 = elf_get_num(S, 5);

	Color color = r_color;

	rSetTopology(J, MODE_TRIANGLES);
	Vertex2D *vertices = rReserveVertices(J, 3);
	vertices[0] = (Vertex2D){{x0,y0},{0,0},color};
	vertices[1] = (Vertex2D){{x1,y1},{1,1},color};
	vertices[2] = (Vertex2D){{x2,y2},{1,1},color};
	return 0;
}

static int L_DrawLine(elf_State *S) {
	JState *J = (JState *) S;

	i32 i = 0;
	f32 x0 = elf_get_num(S, i ++);
	f32 y0 = elf_get_num(S, i ++);
	f32 x1 = elf_get_num(S, i ++);
	f32 y1 = elf_get_num(S, i ++);

	Color color = r_color;

	rSetTexture(J, TEXTURE_DEFAULT);
	rSetTopology(J, MODE_LINES);
	Vertex2D *vertices = rReserveVertices(J, 2);
	vertices[0] = (Vertex2D){{x0,y0},{0,0},color};
	vertices[1] = (Vertex2D){{x1,y1},{1,1},color};
	return 0;
}

static int L_DrawCircle(elf_State *S) {
	JState *J = (JState *) S;

	i32 i = 0;
	f32 x = elf_get_num(S, i ++);
	f32 y = elf_get_num(S, i ++);
	f32 r = elf_get_num(S, i ++);
	f32 v = elf_get_num(S, i ++);

	f32 circumference = TAU * r;
	i32 num_triangles = circumference / v;

	Color color = r_color;
	rSetTexture(J, TEXTURE_DEFAULT);
	rSetTopology(J, MODE_TRIANGLES);

	Vertex2D *vertices = rReserveVertices(J, num_triangles * 3);

	for (i32 i = 0; i < num_triangles; i ++) {
		i32 t = i * 3;
		vertices[t + 0] = (Vertex2D){{x,y},{0,0},color};

		f32 a = ((i + 0.0) / (f32) num_triangles) * TAU;
		f32 ax = x + cos(a) * r;
		f32 ay = y + sin(a) * r;
		vertices[t + 1] = (Vertex2D){{ax,ay},{0,1},color};

		f32 b = ((i + 1.0) / (f32) num_triangles) * TAU;
		f32 bx = x + cos(b) * r;
		f32 by = y + sin(b) * r;
		vertices[t + 2] = (Vertex2D){{bx,by},{1,1},color};
	}
	return 0;
}

static int L_LoadFont(elf_State *S) {
	JState *J = (JState *) S;

	char *name = elf_get_text(S, 0);
	JFont *font = LoadFont(J, name);
	J->font = font;
	return 0;
}


static int L_SolidFill(elf_State *S) {
	JState *J = (JState *) S;
	rSetTexture(J, TEXTURE_DEFAULT);
	return 0;
}

static int L_DrawText(elf_State *S) {
	JState *J = (JState *) S;

	f32 x = elf_get_num(S, 0);
	f32 y = elf_get_num(S, 1);
	char *text = elf_get_text(S, 2);

	JFont *font = J->font;

	u8x4 color = {255, 255, 255, 255};

	rTextureStruct *texture = & J->textures[font->texture];
	f32 inv_resolution_x = 1.0 / texture->resolution.x;
	f32 inv_resolution_y = 1.0 / texture->resolution.y;

	rSetTopology(J, MODE_TRIANGLES);
	rSetTexture(J, font->texture);
	while (*text) {
		i32 token = *text ++;
		i32 index = token - 32;
		stbtt_bakedchar glyph = font->glyphs[index];

		r_f32 src_r = { glyph.x0, glyph.y0, glyph.x1 - glyph.x0, glyph.y1 - glyph.y0 };

		r_f32 dst_r = { x + glyph.xoff, y - (src_r.h + glyph.yoff), src_r.w, src_r.h };

		vec2 r_p0 = { dst_r.x +       0, dst_r.y +       0 };
		vec2 r_p1 = { dst_r.x +       0, dst_r.y + dst_r.h };
		vec2 r_p2 = { dst_r.x + dst_r.w, dst_r.y + dst_r.h };
		vec2 r_p3 = { dst_r.x + dst_r.w, dst_r.y +       0 };

		if (token != ' ') {

			f32 u0 = src_r.x * inv_resolution_x;
			f32 v0 = src_r.y * inv_resolution_y;
			f32 u1 = (src_r.x + src_r.w) * inv_resolution_x;
			f32 v1 = (src_r.y + src_r.h) * inv_resolution_y;

			Vertex2D *vertices = rReserveVertices(J, 6);
			vertices[0]=(Vertex2D){r_p0,{u0,v1},color};
			vertices[1]=(Vertex2D){r_p1,{u0,v0},color};
			vertices[2]=(Vertex2D){r_p2,{u1,v0},color};
			vertices[3]=(Vertex2D){r_p0,{u0,v1},color};
			vertices[4]=(Vertex2D){r_p2,{u1,v0},color};
			vertices[5]=(Vertex2D){r_p3,{u1,v1},color};
		}
		x += glyph.xadvance;
	}
	return 0;
}

static int L_DrawRectangle(elf_State *S) {
	JState *J = (JState *) S;

	f32 x = elf_get_int(S, 0);
	f32 y = elf_get_int(S, 1);
	f32 w = elf_get_int(S, 2);
	f32 h = elf_get_int(S, 3);

	r_i32 src_r = {0, 0, 1, 1};

	Color color = r_color;

	vec2 r_p0 = { x + 0, y + 0 };
	vec2 r_p1 = { x + 0, y + h };
	vec2 r_p2 = { x + w, y + h };
	vec2 r_p3 = { x + w, y + 0 };

	// if (elf_get_num_args(S)-i >= 1) {
	// 	f32 rotation = elf_get_num(S,i ++);
	// 	trans2d rot_trans = trans2d_rotation(rotation,vec2(0,0));
	// 	r_p0 = apply_trans2d(rot_trans,r_p0);
	// 	r_p1 = apply_trans2d(rot_trans,r_p1);
	// 	r_p2 = apply_trans2d(rot_trans,r_p2);
	// 	r_p3 = apply_trans2d(rot_trans,r_p3);
	// 	// todo: pass in trans2d instead!
	// }
	// int flip_x = false;
	// int flip_y = false;
	// if (elf_get_num_args(S)-i >= 2) {
	// 	flip_x = elf_get_int(S,i ++);
	// 	flip_y = elf_get_int(S,i ++);
	// }


	rTextureStruct *texture = & J->textures[J->draw_prox.texture];
	f32 inv_resolution_x = 1.0 / texture->resolution.x;
	f32 inv_resolution_y = 1.0 / texture->resolution.y;

	f32 u0 = src_r.x * inv_resolution_x;
	f32 v0 = src_r.y * inv_resolution_y;
	f32 u1 = (src_r.x + src_r.w) * inv_resolution_x;
	f32 v1 = (src_r.y + src_r.h) * inv_resolution_y;

	// vec2 dst_pos = vec2(dst_r.x,dst_r.y);
	// vec2 translation = vec2_add(r_offset,dst_pos);
	// r_p0 = vec2_add(translation,vec2_mul(r_scale,r_p0));
	// r_p1 = vec2_add(translation,vec2_mul(r_scale,r_p1));
	// r_p2 = vec2_add(translation,vec2_mul(r_scale,r_p2));
	// r_p3 = vec2_add(translation,vec2_mul(r_scale,r_p3));

	// if (flip_x) {
	// 	f32 temp = u0;
	// 	u0 = u1;
	// 	u1 = temp;
	// }

	rSetTopology(J, MODE_TRIANGLES);
	Vertex2D *vertices = rReserveVertices(J, 6);
	vertices[0]=(Vertex2D){r_p0,{u0,v1},color};
	vertices[1]=(Vertex2D){r_p1,{u0,v0},color};
	vertices[2]=(Vertex2D){r_p2,{u1,v0},color};
	vertices[3]=(Vertex2D){r_p0,{u0,v1},color};
	vertices[4]=(Vertex2D){r_p2,{u1,v0},color};
	vertices[5]=(Vertex2D){r_p3,{u1,v1},color};
	return 0;
}


int main() {
	JState jam = {};
	elf_init(&jam.R, &jam.M);


	{
		timeBeginPeriod(1);
		{
			LARGE_INTEGER i = {};
			QueryPerformanceFrequency(&i);
			f64 frequency = i.QuadPart;
			os.clocks_to_seconds = (f64) 1.0 / frequency;
		}
	}


	NameFunctionPair lib[] = {
		{ "InitWindow", L_InitWindow },
		{ "PollWindow", L_PollWindow },
		// { "LoadTexture", L_LoadTexture },
		{ "LoadFont", L_LoadFont },
		{ "Button", L_Button },
		{ "Clear", L_Clear },
		{ "GetCursorX", L_GetCursorX },
		{ "GetCursorY", L_GetCursorY },
		{ "SetScale", L_SetScale },
		{ "SetOffset", L_SetOffset },
		{ "SetRotation", L_SetRotation },
		{ "SetCenter", L_SetCenter },
		{ "SetColor", L_SetColor },
		{ "DrawText", L_DrawText },
		{ "SolidFill", L_SolidFill },
		{ "DrawRectangle", L_DrawRectangle },
		{ "DrawTriangle", L_DrawTriangle },
		{ "DrawLine", L_DrawLine },
		{ "DrawCircle", L_DrawCircle },
		// {"elf.jam.audio", l_audio},
		// {"elf.jam.audio_play", l_audio_play},
		// {"elf.jam.audio_load", l_audio_load},
		// {"elf.jam.audio_stop", l_audio_stop},
		// {"elf.jam.load_image", j_load_image},
		// {"elf.jam.get_image_pixel", j_get_image_pixel},
		// {"elf.jam.get_image_size", j_get_image_size},
		// {"elf.jam.get_texture_size", l_get_texture_size},
		// {"elf.jam.get_input", l_get_input},
	};

	elf_Table *globals = jam.M.globals;
	for (int i = 0; i < COUNTOF(lib); i ++) {
		elf_String *name = elf_alloc_string(&jam.R, lib[i].name);
		elf_table_set(globals, VALUE_STRING(name), VALUE_FUNCTION(lib[i].fn));
	}

	elf_add_proc(&jam.R, core_lib_load_file);
	elf_add_nil(&jam.R);
	elf_new_string(&jam.R, "launch.elf");
	elf_call(&jam.R,2,0);
}


#if 0

// static int l_get_texture_size(elf_State *S) {
// 	rTextureStruct *texture_o = j_get(S,0,JAM_TEXTURE);
// 	if(!texture_o){
// 		elf_error(S,0,elf_tpf("invalid argument, is: %s", tag2s[elf_get_tag(S,0)]));
// 	}
// 	elf_Table *size_table = elf_new_table(S);
// 	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"x")),VALUE_INTEGER(texture_o->resolution.x));
// 	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"y")),VALUE_INTEGER(texture_o->resolution.y));
// 	elf_add_table(S,size_table);
// 	return 1;
// }

// static int j_get_image_size(elf_State *S) {
// 	jam_Image *image = (jam_Image *) elf_get_object(S,0);
// 	elf_Table *size_table = elf_new_table(S);
// 	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"x")),VALUE_INTEGER(image->size.x));
// 	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"y")),VALUE_INTEGER(image->size.y));
// 	elf_add_table(S,size_table);
// 	return 1;
// }

// static int j_get_image_pixel(elf_State *S) {
// 	jam_Image *image = (jam_Image *) elf_get_object(S,0);
// 	i32 x = elf_get_int(S,1);
// 	i32 y = elf_get_int(S,2);
// 	if(x < 0 || x >= image->size.x || y < 0 || y >= image->size.y) {
// 		elf_error(S,0,"invalid pixel coordinates");
// 	}
// 	Color pixel = image->pixels[image->size.x * y + x];
// 	u32 packed = pixel.r << 24 | pixel.g << 16 | pixel.b << 8 | pixel.a;
// 	elf_add_int(S,packed);
// 	return 1;
// }
// static int j_load_image(elf_State *S) {
// 	int i = 0;
// 	char *name = elf_get_text(S,i++);
// 	Color *pixels = 0;
// 	int n=0,x=0,y=0;
// 	if(name){
// 		pixels = (Color *) stbi_load(name,&x,&y,&n,4);
// 	}
// 	jam_Image *image = j_new(S,JAM_IMAGE,sizeof(*image));
// 	image->size.x = x;
// 	image->size.y = y;
// 	image->pixels = pixels;
// 	return 1;
// }

// static int L_LoadTexture(elf_State *S) {
// 	JState *J = (JState *) S;

// 	int i = 0;
// 	char *name = elf_get_text(S,i++);

// 	b32 do_replace = false;
// 	Color replace_src = {};
// 	Color replace_dst = {};
// 	if (elf_get_num_args(S) - i >= 4) {
// 		i = _get_color(S,i,&replace_src);
// 		if (elf_get_num_args(S) - i >= 4) {
// 			i = _get_color(S,i,&replace_dst);
// 		}
// 		do_replace = true;
// 	}

// 	Color *pixels = 0;
// 	int n = 0,w = 0,h = 0;
// 	if(name){
// 		pixels = (Color *) stbi_load(name,&w,&h,&n,4);
// 	}

// 	if(pixels){
// 		if(do_replace){
// 			for(i64 i = 0; i < w * h; i ++) {
// 				if(pixels[i].x_i32 == replace_src.x_i32) {
// 					pixels[i] = replace_dst;
// 				}
// 			}
// 		}

// 		rTextureStruct *texture_o = j_new(S,JAM_TEXTURE,sizeof(*texture_o));
// 		rInstallTexture(J, texture_o, (vec2i){w, h}, pixels);

// 		free(pixels);

// 		elf_add_object(S, (elf_Object *) texture_o);
// 	}else{
// 		elf_add_nil(S);
// 	}
// 	return 1;
// }

//	static int L_Vertex(elf_State *S) {
//		JState *J = (JState *) S;
//
//		f32 x = elf_get_num(S, i ++);
//		f32 y = elf_get_num(S, i ++);
//	}
//
// audio
//

static int l_audio(elf_State *S) {
	ma_result error = ma_engine_init(NULL,&J->audio.engine);
	elf_add_int(S,error==MA_SUCCESS);
	return 1;
}

static int l_audio_load(elf_State *S) {

	char *name = elf_get_text(S,0);

	jam_Sound *obj = j_new(S,JAM_SOUND,sizeof(*obj));

	char *name_stable = malloc(strlen(name) + 1);
	strcpy(name_stable,name);

	ma_result result = ma_sound_init_from_file(&J->audio.engine, name_stable, 0, NULL, NULL, &obj->sound);

	if (result != MA_SUCCESS) {
		elf_error(S,0,"failed to load sound");
	}
	return 1;
}

static int l_audio_play(elf_State *S) {
	jam_Audio *audio = &J->audio;
	if (elf_get_tag(S,0) == elf_tag_userobj) {
		jam_Sound *obj = j_get(S,0,JAM_SOUND);
		f32 pitch = 1.0;
		if (elf_get_num_args(S) > 1) {
			pitch = elf_get_num(S,1);
		}

		jam_Sound *to_play = 0;
		i32 open_voice_slot = -1;
		for (i32 i = 0; i < _countof(audio->voices); i ++) {
			jam_Sound *voice = audio->voices[i];
			if (voice) {
				if (ma_sound_at_end(&voice->sound)) {
					to_play = voice;
					break;
				}
			} else {
				open_voice_slot = i;
				break;
			}
		}
		if (to_play) {
			// todo: only uninit if the sound that we found isn't
			// the same one we're trying to play
			ma_sound_uninit(&to_play->sound);
			ma_result result = ma_sound_init_copy(&audio->engine,&obj->sound,0,0,&to_play->sound);
		} else if (open_voice_slot != -1) {
			to_play = j_new(S,JAM_SOUND,sizeof(*to_play));
			ma_result result = ma_sound_init_copy(&audio->engine,&obj->sound,0,0,&to_play->sound);
			audio->voices[open_voice_slot] = to_play;
		} else {
			elf_debug_log("too many sounds playing simultaneously");
		}
		if (to_play) {
			ma_sound_set_pitch(&to_play->sound,pitch);
			ma_sound_start(&to_play->sound);
		}
	} else {
		char *name = elf_get_text(S,0);
		ma_engine_play_sound(&J->audio.engine,name, NULL);
	}
	return 0;
}

static int l_audio_stop(elf_State *S) {
	jam_Sound *obj = j_get(S,0,JAM_SOUND);
	ma_sound_stop(&obj->sound);
	return 0;
}
#endif
