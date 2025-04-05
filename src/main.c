#define _DEBUG
#include "elf.c"

// <3
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#if defined(_WIN32)
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
#endif

#include "jam.h"
#include "win32_platform.c"
#include "d3d11_renderer.c"


#define MODE_NONE 		D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED
#define MODE_TEXTURES 	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
#define MODE_LINES 		D3D11_PRIMITIVE_TOPOLOGY_LINELIST

jam_State     JAM;

i32           r_mode;
jam_Texture  *r_texture;
Vertex2D     *r_mem_vertices;
i32           r_num_vertices;
i32           r_max_vertices;

vec2      	  r_scale;
vec2      	  r_offset;
f32       	  r_rotation;
vec2      	  r_center;

typedef BOOL win32_SetProcessDPIAwarenessContext(void* value);
typedef UINT win32_GetDPIForWindow(HWND hwnd);
#define WIN32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((void*)-4)

#define WIN32_WINDOW_CLASS_NAME L"graphical-window"


enum {
	DOWN_BIT       = 1,
	PRESSED_BIT    = 2,
	RELEASED_BIT   = 4,
};

enum {
	BUTTON_NONE = 0,

	BUTTON_MOUSE_LEFT,
	BUTTON_MOUSE_MIDDLE,
	BUTTON_MOUSE_RIGHT,
	BUTTON_MOUSE_COUNT,

	BUTTON_MENU,
	BUTTON_BACK,
	BUTTON_DEBUG,

	BUTTON_LEFT,
	BUTTON_UP,
	BUTTON_RIGHT,
	BUTTON_DOWN,

	BUTTON_DPAD_LEFT,
	BUTTON_DPAD_UP,
	BUTTON_DPAD_RIGHT,
	BUTTON_DPAD_DOWN,

	BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4,
	BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9,

	BUTTON_JUMP,
	BUTTON_DASH,
	BUTTON_USE,
	BUTTON_SHOW_MINIMAP,

	BUTTON_COUNT,
};

typedef u8 Button;

struct {
	HWND                   window;
	Button                 keyboard[256];
	Button                 controller[16];
	HCURSOR                cursor;
	b32                    close_window;
	f64                    clocks_to_seconds;
	// vec2i                  mouse_xy;
	// vec2i                  mouse_wheel;
} global os;


static inline i64 os_clock() {
	LARGE_INTEGER p = {};
	QueryPerformanceCounter(&p);
	return p.QuadPart;
}

static void os_error_dialog(char *msg) {
	if (!msg) {
		DWORD error = GetLastError();
		LPSTR message = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
		, NULL, error, 0, (LPSTR) &message, 0, NULL);
		msg = message;
	}
	MessageBoxA(NULL, msg, "Error", MB_OK | MB_ICONERROR);
}

static inline void onoff(Button *btn, i32 on) {
	*btn = on | (*btn ^ 1) << (on + 1);
}


static void _r_draw(i32 type, Vertex2D *mem_vertices, i32 num_vertices) {
	ASSERT(type != MODE_NONE);
	ASSERT(num_vertices != 0);

	i32 offset = _r_write_vertices(&JAM, mem_vertices, num_vertices);
	switch (type) {
		case MODE_TEXTURES: {
			ASSERT(num_vertices % 6 == 0);

			D3D11_PRIMITIVE_TOPOLOGY topology;
			ID3D11DeviceContext_IAGetPrimitiveTopology(JAM.context, &topology);
			ASSERT(topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			ID3D11ShaderResourceView *inputs[1] = {};
			ID3D11DeviceContext_PSGetShaderResources(JAM.context, 0, 1, inputs);
			ASSERT(inputs[0] != 0);

			ID3D11DeviceContext_Draw(JAM.context, num_vertices, offset);
		} break;
		case MODE_LINES: {
			ASSERT(num_vertices % 2 == 0);

			D3D11_PRIMITIVE_TOPOLOGY topology;
			ID3D11DeviceContext_IAGetPrimitiveTopology(JAM.context, &topology);
			ASSERT(topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			ID3D11ShaderResourceView *inputs[1] = {};
			ID3D11DeviceContext_PSGetShaderResources(JAM.context, 0, 1, inputs);
			ASSERT(inputs[0] != 0);

			ID3D11DeviceContext_Draw(JAM.context, num_vertices, offset);
		} break;
	}
}

static void _jam_flush_render_pass() {
	if (r_mode != MODE_NONE && r_num_vertices != 0) {
		_r_draw(r_mode, r_mem_vertices, r_num_vertices);
		r_num_vertices = 0;
	}
}

static void _r_change_mode(D3D11_PRIMITIVE_TOPOLOGY mode) {
	if (r_mode != mode) {
		_jam_flush_render_pass();

		ID3D11DeviceContext_IASetPrimitiveTopology(JAM.context, mode);
		r_mode = mode;
	}
}

void _r_change_texture(jam_Texture *texture) {
	if(!texture) texture = &JAM.fallback_texture;

	if (r_texture != texture) {
		_jam_flush_render_pass();

		r_texture = texture;

		ID3D11ShaderResourceView *input = texture->view;
		ID3D11SamplerState *sampler = texture->sampler;
		ID3D11DeviceContext_PSSetShaderResources(JAM.context, 0, 1, &input);
		ID3D11DeviceContext_PSSetSamplers(JAM.context, 0, 1, &sampler);
	}
}




static void *j_get(elf_State *S, int arg, jam_type type) {
	if (elf_get_tag(S,arg) != elf_tag_userobj) {
		elf_error(S,0,elf_tpf("jam expects '%s' object", j_obj2str[type]));
	}
	jam_Object *obj = (jam_Object *) elf_get_object(S,arg);
	if (obj->type != type) {
		elf_error(S,0,elf_tpf("jam expects '%s' object", j_obj2str[type]));
	}
	return obj;
}

// todo: mini audio expects objects to get deallocated
// through their backend, so we can't just use GC, unless
// the GC called some _collect function or something, which
// elf doesn't do...
static void *j_new(elf_State *S, int type, int size) {
	// jam_Object *obj = (jam_Object *) elf_new_object(S,size);
	jam_Object *obj = (jam_Object *) calloc(1,size);
	elf_add_object(S, (elf_Object*) obj);
	obj->type = type;
	ASSERT(size >= sizeof(*obj));
	return obj;
}


int _get_rect(elf_State *S, int i, r_i32 *rect) {
	rect->x = elf_get_int(S,i++);
	rect->y = elf_get_int(S,i++);
	rect->w = elf_get_int(S,i++);
	rect->h = elf_get_int(S,i++);
	return i;
}

int _get_color(elf_State *S, int i, u8x4 *color) {
	color->r = elf_get_int(S,i++) & 255;
	color->g = elf_get_int(S,i++) & 255;
	color->b = elf_get_int(S,i++) & 255;
	color->a = elf_get_int(S,i++) & 255;
	return i;
}

static void _jam_update_globals(elf_State *S) {
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.video.res_x")),VALUE_INTEGER(JAM.base_resolution.x));
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.video.res_y")),VALUE_INTEGER(JAM.base_resolution.y));
	// elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.input.mouse_x")),VALUE_INTEGER(JAM.mouse_xy.x));
	// elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.input.mouse_y")),VALUE_INTEGER(JAM.base_resolution.y - JAM.mouse_xy.y));
}

static LRESULT win32_window_proc(HWND window, UINT msg, WPARAM w, LPARAM l);

static int j_video(elf_State *S) {
	elf_debug_log("jam: begin video mode");

	int i = 0;
	char *name       = elf_get_text(S,i++);
	int resolution_x = elf_get_int(S,i++);
	int resolution_y = elf_get_int(S,i++);
	int window_scale = elf_get_int(S,i++);

	if (window_scale == 0) window_scale = 1;

#if defined(_WIN32)
	r_mem_vertices = VirtualAlloc(0, MEGABYTES(128), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	r_max_vertices = MEGABYTES(128) / sizeof(*r_mem_vertices);
	r_num_vertices = 0;
	r_scale.x = 1;
	r_scale.y = 1;

	i32 screen_x = GetSystemMetrics(SM_CXSCREEN);
	i32 screen_y = GetSystemMetrics(SM_CYSCREEN);
   // i32 max_scale_x = screen_x / (f32) BASE_RES_X;
   // i32 max_scale_y = screen_y / (f32) BASE_RES_Y;
   // i32 scale = MIN(max_scale_x, max_scale_y);
	i32 client_size_x = resolution_x * window_scale;
	i32 client_size_y = resolution_y * window_scale;

	RECT window_rect = {
		.left = 0,
		.top = 0,
		.right = client_size_x,
		.bottom = client_size_y,
	};

	b32 error = AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);
	ASSERT(error != 0);

	i32 size_x = window_rect.right - window_rect.left;
	i32 size_y = window_rect.bottom - window_rect.top;

	i32 window_x = screen_x * 0.5 - size_x * 0.5;
	i32 window_y = screen_y * 0.5 - size_y * 0.5;

	wchar_t window_title[256] = {};
	MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,name,-1,window_title,sizeof(window_title));

	HWND window = CreateWindowExW(0
	, WIN32_WINDOW_CLASS_NAME, window_title
	, WS_OVERLAPPEDWINDOW, window_x, window_y, size_x, size_y
	, NULL, NULL, GetModuleHandle(0), NULL);

	if (!IsWindow(window)) {
		os_error_dialog(0);
	}

	ShowWindow(window, SW_SHOW);

	os.window = window;
#endif


	init_renderer((Init_Renderer) {
		.jam = &JAM,
		.window = window,
		.res_x = resolution_x,
		.res_y = resolution_y
	});

	// {
	// 	int n = 0,x = 0,y = 0;
	// 	void *pixels = stbi_load_from_memory(__embedded_font_png_data,__embedded_font_png_size,&x,&y,&n,4);
	// 	JAM.default_font_texture = r_texture(&JAM,FORMAT_RGBA_U8,x,y,pixels);
	// }

	_jam_update_globals(S);
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.DOWN_BIT")),VALUE_INTEGER(1));
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.PRESSED_BIT")),VALUE_INTEGER(2));
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.RELEASED_BIT")),VALUE_INTEGER(4));

	JAM.target_seconds_to_sleep = 1.0 / 60.0;
	JAM.begin_cycle_clock = os_clock();
	return 0;
}

static int l_get_input(elf_State *S){
	i32 key = elf_get_int(S,0);
	// i32 was = JAM.was_input[key];
	// b32 now = JAM.now_input[key];
	// b32 is_pressed = now && !was;
	// b32 is_released = !now && was;
	// b32 input = is_released << 2 | is_pressed << 1 | now << 0;
	// elf_add_int(S,input);
	elf_add_int(S, 0);
	return 1;
}

static int jl_cycle(elf_State *S) {
#if defined(_WIN32)
	{
		MSG window_message = {};
		while(PeekMessage(&window_message, os.window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&window_message);
			DispatchMessageW(&window_message);
		}

		RECT window_r;
		GetClientRect(os.window, &window_r);
		JAM.window_dimensions.x = window_r.right - window_r.left;
		JAM.window_dimensions.y = window_r.bottom - window_r.top;
	}
#endif

	_jam_flush_render_pass();
	_jam_renderer_cycle(&JAM);
	_jam_update_globals(S);
	r_texture = 0;

	// todo: this seems to work pretty good,
	// but we have to actually do it properly...
	S->G.paused = false;
	_gc_check(S,0);
	S->G.paused = true;

	// todo:
	i64 end_cycle_clock = os_clock();
	i64 elapsed_clocks = end_cycle_clock - JAM.begin_cycle_clock;
	f64 elapsed_seconds = elapsed_clocks * os.clocks_to_seconds;
	JAM.pending_seconds_to_sleep += JAM.target_seconds_to_sleep - elapsed_seconds;


	f64 clock_accuracy = 1.0 / 1000.0;
	JAM.begin_cycle_clock = os_clock();
	while (JAM.pending_seconds_to_sleep > clock_accuracy) {
		Sleep(JAM.pending_seconds_to_sleep * 1000);
		JAM.begin_cycle_clock = os_clock();
		JAM.pending_seconds_to_sleep -= (JAM.begin_cycle_clock - end_cycle_clock) * os.clocks_to_seconds;
	}

	elf_add_int(S,!os.close_window);
	return 1;
}

static int l_get_texture_size(elf_State *S) {
	jam_Texture *texture_o = j_get(S,0,JAM_TEXTURE);
	if(!texture_o){
		elf_error(S,0,elf_tpf("invalid argument, is: %s", tag2s[elf_get_tag(S,0)]));
	}
	elf_Table *size_table = elf_new_table(S);
	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"x")),VALUE_INTEGER(texture_o->size.x));
	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"y")),VALUE_INTEGER(texture_o->size.y));
	elf_add_table(S,size_table);
	return 1;
}

static int j_get_image_size(elf_State *S) {
	jam_Image *image = (jam_Image *) elf_get_object(S,0);
	elf_Table *size_table = elf_new_table(S);
	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"x")),VALUE_INTEGER(image->size.x));
	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"y")),VALUE_INTEGER(image->size.y));
	elf_add_table(S,size_table);
	return 1;
}

static int j_get_image_pixel(elf_State *S) {
	jam_Image *image = (jam_Image *) elf_get_object(S,0);
	i32 x = elf_get_int(S,1);
	i32 y = elf_get_int(S,2);
	if(x < 0 || x >= image->size.x || y < 0 || y >= image->size.y) {
		elf_error(S,0,"invalid pixel coordinates");
	}
	Color pixel = image->pixels[image->size.x * y + x];
	u32 packed = pixel.r << 24 | pixel.g << 16 | pixel.b << 8 | pixel.a;
	elf_add_int(S,packed);
	return 1;
}
static int j_load_image(elf_State *S) {
	int i = 0;
	char *name = elf_get_text(S,i++);
	Color *pixels = 0;
	int n=0,x=0,y=0;
	if(name){
		pixels = (Color *) stbi_load(name,&x,&y,&n,4);
	}
	jam_Image *image = j_new(S,JAM_IMAGE,sizeof(*image));
	image->size.x = x;
	image->size.y = y;
	image->pixels = pixels;
	return 1;
}

static int j_load_texture(elf_State *S) {
	int i = 0;
	char *name = elf_get_text(S,i++);

	b32 do_replace = false;
	Color replace_src = {};
	Color replace_dst = {};
	if (elf_get_num_args(S) - i >= 4) {
		i = _get_color(S,i,&replace_src);
		if (elf_get_num_args(S) - i >= 4) {
			i = _get_color(S,i,&replace_dst);
		}
		do_replace = true;
	}

	Color *pixels = 0;
	int n = 0,w = 0,h = 0;
	if(name){
		pixels = (Color *) stbi_load(name,&w,&h,&n,4);
	}

	if(pixels){
		if(do_replace){
			for(i64 i = 0; i < w * h; i ++) {
				if(pixels[i].x_i32 == replace_src.x_i32) {
					pixels[i] = replace_dst;
				}
			}
		}
		jam_Texture *texture_o = j_new(S,JAM_TEXTURE,sizeof(*texture_o));
		r_equip_texture(&JAM,texture_o,w,h,pixels);

		free(pixels);

		elf_add_object(S, (elf_Object *) texture_o);
	}else{
		elf_add_nil(S);
	}
	return 1;
}

int jl_render_rotation(elf_State *S) {
	r_rotation = elf_get_num(S,0);
	return 0;
}

int jl_render_center(elf_State *S) {
	r_center.x = elf_get_num(S,0);
	r_center.y = elf_get_num(S,1);
	return 0;
}

int jl_render_scale(elf_State *S) {
	r_scale.x = elf_get_num(S,0);
	r_scale.y = elf_get_num(S,1);
	return 0;
}

int jl_render_offset(elf_State *S) {
	r_offset.x = elf_get_num(S,0);
	r_offset.y = elf_get_num(S,1);
	return 0;
}

static int j_draw_line(elf_State *S) {
	i32 i = 0;
	f32 x0 = elf_get_num(S, i ++);
	f32 y0 = elf_get_num(S, i ++);
	f32 x1 = elf_get_num(S, i ++);
	f32 y1 = elf_get_num(S, i ++);
	Color color = {255,255,255,255};
	if (elf_get_num_args(S)-i >= 4) {
		i = _get_color(S, i, &color);
	}
	_r_change_mode(MODE_LINES);
	_r_change_texture(&JAM.fallback_texture);
	r_mem_vertices[r_num_vertices ++] = (Vertex2D){{x0,y0},{0,0},color};
	r_mem_vertices[r_num_vertices ++] = (Vertex2D){{x1,y1},{1,1},color};
	return 0;
}

// todo: if the idea is to encourage users to do sprite
// batching, or at least make that process easier and
// more intuitive, it would be best to remove the texture
// argument from here
static int j_draw_sprite(elf_State *S) {
	int i = 0;

	int nargs = elf_get_num_args(S);

	jam_Texture *texture = 0;
	if(elf_get_tag(S,0) != elf_tag_nil) {
		texture = j_get(S,i++,JAM_TEXTURE);
	} else {
		i ++;
	}
	if(!texture) texture = & JAM.fallback_texture;

	r_i32 src_r;
	i = _get_rect(S, i, &src_r);
	if(src_r.size.x == 0) src_r.size.x = texture->size.x;
	if(src_r.size.y == 0) src_r.size.y = texture->size.y;

	r_i32 dst_r;
	i = _get_rect(S, i, &dst_r);

	if(dst_r.size.x == 0) dst_r.size.x = src_r.size.x;
	if(dst_r.size.y == 0) dst_r.size.y = src_r.size.y;

	Color color = {255,255,255,255};
	if (elf_get_num_args(S)-i >= 4) {
		i = _get_color(S, i, &color);
	}

	vec2 r_p0 = { 0, };
	vec2 r_p1 = { 0, dst_r.h };
	vec2 r_p2 = { dst_r.w, dst_r.h };
	vec2 r_p3 = { dst_r.w, 0};

	if (elf_get_num_args(S)-i >= 1) {
		f32 rotation = elf_get_num(S,i ++);

		trans2d rot_trans = trans2d_rotation(rotation,vec2(0,0));
		r_p0 = apply_trans2d(rot_trans,r_p0);
		r_p1 = apply_trans2d(rot_trans,r_p1);
		r_p2 = apply_trans2d(rot_trans,r_p2);
		r_p3 = apply_trans2d(rot_trans,r_p3);
		// todo: pass in trans2d instead!
	}

	int flip_x = false;
	int flip_y = false;
	if (elf_get_num_args(S)-i >= 2) {
		flip_x = elf_get_int(S,i ++);
		flip_y = elf_get_int(S,i ++);
	}

	_r_change_mode(MODE_TEXTURES);
	_r_change_texture(texture);

	f32 inv_src_w = 1.0 / texture->size.x;
	f32 inv_src_h = 1.0 / texture->size.y;

	f32 u0 = src_r.x * inv_src_w;
	f32 v0 = src_r.y * inv_src_h;
	f32 u1 = (src_r.x + src_r.w) * inv_src_w;
	f32 v1 = (src_r.y + src_r.h) * inv_src_h;

	vec2 dst_pos = vec2(dst_r.x,dst_r.y);
	vec2 translation = vec2_add(r_offset,dst_pos);
	// todo: we're doing too many multiplies, we can do
	// 2 multiplies on the actual rectangle dimensions
	// instead of the 8 we're doing here.
	r_p0 = vec2_add(translation,vec2_mul(r_scale,r_p0));
	r_p1 = vec2_add(translation,vec2_mul(r_scale,r_p1));
	r_p2 = vec2_add(translation,vec2_mul(r_scale,r_p2));
	r_p3 = vec2_add(translation,vec2_mul(r_scale,r_p3));

	if (flip_x) {
		f32 temp = u0;
		u0 = u1;
		u1 = temp;
	}

	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p0,{u0,v1},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p1,{u0,v0},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p2,{u1,v0},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p0,{u0,v1},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p2,{u1,v0},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p3,{u1,v1},color};
	return 0;
}

//
// audio
//

static int l_audio(elf_State *S) {
	ma_result error = ma_engine_init(NULL,&JAM.audio.engine);
	elf_add_int(S,error==MA_SUCCESS);
	return 1;
}

static int l_audio_load(elf_State *S) {

	char *name = elf_get_text(S,0);

	jam_Sound *obj = j_new(S,JAM_SOUND,sizeof(*obj));

	char *name_stable = malloc(strlen(name) + 1);
	strcpy(name_stable,name);

	ma_result result = ma_sound_init_from_file(&JAM.audio.engine, name_stable, 0, NULL, NULL, &obj->sound);

	if (result != MA_SUCCESS) {
		elf_error(S,0,"failed to load sound");
	}
	return 1;
}

static int l_audio_play(elf_State *S) {
	jam_Audio *audio = &JAM.audio;
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
		ma_engine_play_sound(&JAM.audio.engine,name, NULL);
	}
	return 0;
}

static int l_audio_stop(elf_State *S) {
	jam_Sound *obj = j_get(S,0,JAM_SOUND);
	ma_sound_stop(&obj->sound);
	return 0;
}

int main() {
	elf_State R = {};
	elf_Module M = {};
	elf_init(&R, &M);

	// init platform stuff
	{
		HMODULE user32 = LoadLibraryA("user32.dll");
		if (user32) {
			win32_SetProcessDPIAwarenessContext *set_dpi_awareness = (win32_SetProcessDPIAwarenessContext*)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
			if (set_dpi_awareness) {
				set_dpi_awareness(WIN32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			}
			FreeLibrary(user32);
		}
	}
	{
		WNDCLASSEXW window_class = {
			.cbSize = sizeof(window_class),
			.lpfnWndProc = win32_window_proc,
			.hInstance = GetModuleHandle(0),
			.lpszClassName = WIN32_WINDOW_CLASS_NAME,
			.hCursor = LoadCursorA(NULL, IDC_ARROW),
			.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(1)),
		};
		if (!RegisterClassExW(&window_class)) {
			os_error_dialog(0);
		}
	}
	{
		timeBeginPeriod(1);
		{
			LARGE_INTEGER i = {};
			QueryPerformanceFrequency(&i);
			f64 frequency = i.QuadPart;
			os.clocks_to_seconds = (f64) 1.0 / frequency;
		}
	}



	elf_Table *globals = M.globals;
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.video")),VALUE_FUNCTION(j_video));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.audio")),VALUE_FUNCTION(l_audio));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.audio_play")),VALUE_FUNCTION(l_audio_play));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.audio_load")),VALUE_FUNCTION(l_audio_load));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.audio_stop")),VALUE_FUNCTION(l_audio_stop));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.cycle")),VALUE_FUNCTION(jl_cycle));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.load_image")),VALUE_FUNCTION(j_load_image));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.get_image_pixel")),VALUE_FUNCTION(j_get_image_pixel));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.get_image_size")),VALUE_FUNCTION(j_get_image_size));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.load_texture")),VALUE_FUNCTION(j_load_texture));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.get_texture_size")),VALUE_FUNCTION(l_get_texture_size));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.draw_sprite")),VALUE_FUNCTION(j_draw_sprite));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.draw_line")),VALUE_FUNCTION(j_draw_line));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.render_scale")),VALUE_FUNCTION(jl_render_scale));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.render_offset")),VALUE_FUNCTION(jl_render_offset));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.render_rotation")),VALUE_FUNCTION(jl_render_rotation));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.render_center")),VALUE_FUNCTION(jl_render_center));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.get_input")),VALUE_FUNCTION(l_get_input));

	elf_add_proc(&R,core_lib_load_file);
	elf_add_nil(&R);
	elf_new_string(&R,"launch.elf");
	elf_call(&R,2,0);
}

#if defined(_WIN32)
static LRESULT win32_window_proc(HWND window, UINT msg, WPARAM w, LPARAM l) {
	LRESULT yield = FALSE;

	switch(msg) {
		case WM_CLOSE: {
			TRACELOG("Close Window");
			os.close_window = true;
		} break;
		case WM_ENTERSIZEMOVE:
		case WM_EXITSIZEMOVE: {
         // app->resizing = msg == WM_ENTERSIZEMOVE;
		} break;
		case WM_LBUTTONUP: {
			onoff(&os.keyboard[VK_LBUTTON], FALSE);
		} break;
		case WM_MBUTTONUP: {
			onoff(&os.keyboard[VK_MBUTTON], FALSE);
		} break;
		case WM_RBUTTONUP: {
			onoff(&os.keyboard[VK_RBUTTON], FALSE);
		} break;
		case WM_SYSKEYUP: case WM_KEYUP: {
			onoff(&os.keyboard[w], FALSE);
		} break;
		case WM_LBUTTONDOWN: {
			onoff(&os.keyboard[VK_LBUTTON], TRUE);
		} break;
		case WM_MBUTTONDOWN: {
			onoff(&os.keyboard[VK_MBUTTON], TRUE);
		} break;
		case WM_RBUTTONDOWN: {
			onoff(&os.keyboard[VK_RBUTTON], TRUE);
		} break;
		case WM_SYSKEYDOWN: case WM_KEYDOWN: {
			onoff(&os.keyboard[w], TRUE);
		} break;
		case WM_MOUSEMOVE: {
			// os.mouse_xy.x = LOWORD(l);
			// os.mouse_xy.y = HIWORD(l);
		} break;
		case WM_MOUSEWHEEL: {
			// os.mouse_wheel.y = GET_WHEEL_DELTA_WPARAM(w);
			// os.mouse_wheel.y = os.mouse_wheel.y < 0 ? -1 : 1;
		} break;
		case WM_MOUSEHWHEEL: {
			// os.mouse_wheel.x = HIWORD(w);
		} break;
		case WM_CHAR: {
		} break;
		// case WM_SETCURSOR: {
		// 	SetCursor(os.cursor);
		// } break;
		// case WM_KILLFOCUS: {
		// 	ReleaseCapture();
		// } break;
      // case WM_DPICHANGED: {
      // } break;
		default: {
			yield = DefWindowProcW(window, msg, w, l);
		} break;
	}

	return yield;
}
#endif

