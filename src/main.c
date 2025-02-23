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

jam_State     JAM;
jam_Texture   r_texture;
// todo: the reason behind having an intermediate
// buffer is due to how the rendering API works,
// perhaps this should be abstracted in the API
// itself...
Vertex2D     *r_mem_vertices;
i32           r_num_vertices;
i32           r_max_vertices;
// these persist across draw calls, I don't do
// them on the GPU because this way is simpler,
// but maybe eventually for other more rendering
// pipelines we may want to move away from this
// since does incur a cost...
vec2 	        r_scale;
vec2 	        r_offset;
// todo: instead compute the basis from the
// rotation
f32 	        r_rotation;
vec2 	        r_center;


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

void r_flush() {
	if (r_num_vertices) {
		r_texture_pass(&JAM,r_texture,r_mem_vertices,r_num_vertices);
		r_num_vertices = 0;
	}
}
void _jr_set_texture(jam_Texture texture) {
	if (r_texture.texture != texture.texture) {
		r_flush();
		r_texture = texture;
	}
}

static int l_audio(elf_State *S) {
	ma_result error = ma_engine_init(NULL,&JAM.sound_engine);
	elf_add_int(S,error==MA_SUCCESS);
	return 1;
}

static int l_audio_play(elf_State *S) {
	char *name = elf_get_text(S,0);
	ma_engine_play_sound(&JAM.sound_engine,name, NULL);
	return 0;
}

static void _j_cycle_globals(elf_State *S) {
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.video.res_x")),VALUE_INTEGER(JAM.base_resolution.x));
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.video.res_y")),VALUE_INTEGER(JAM.base_resolution.y));
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.input.mouse_x")),VALUE_INTEGER(JAM.mouse_xy.x));
	// todo:
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.input.mouse_y")),VALUE_INTEGER(JAM.base_resolution.y - JAM.mouse_xy.y));
}

static int j_video(elf_State *S) {
	int i = 0;
	char *name       = elf_get_text(S,i++);
	int base_res_x   = elf_get_int(S,i++);
	int base_res_y   = elf_get_int(S,i++);
	int window_scale = elf_get_int(S,i++);

	os_init();

	r_mem_vertices = VirtualAlloc(0, MEGABYTES(32), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	r_max_vertices = MEGABYTES(32) / sizeof(*r_mem_vertices);
	r_num_vertices = 0;
	r_scale.x = 1;
	r_scale.y = 1;


	if (window_scale == 0) window_scale = 1;
	Window_Token window_token = equip_window(&JAM,(Equip_Window){
		.name = name,
		.size_x = base_res_x * window_scale,
		.size_y = base_res_y * window_scale
	});
	r_equip(&JAM,window_token,(Equip_Renderer){
		.res_x = base_res_x,
		.res_y = base_res_y
	});

	// {
	// 	int n = 0,x = 0,y = 0;
	// 	void *pixels = stbi_load_from_memory(__embedded_font_png_data,__embedded_font_png_size,&x,&y,&n,4);
	// 	JAM.default_font_texture = r_texture(&JAM,FORMAT_RGBA_U8,x,y,pixels);
	// }

	_j_cycle_globals(S);
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.input.keys.KEY_DOWN")),VALUE_INTEGER(1));
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.input.keys.KEY_PRESSED")),VALUE_INTEGER(2));
	elf_table_set(S->M->globals,VALUE_STRING(elf_alloc_string(S,"elf.jam.input.keys.KEY_RELEASED")),VALUE_INTEGER(4));
	return 0;
}

static int l_get_input(elf_State *S){
	i32 key = elf_get_int(S,0);
	i32 was = JAM.was_input[key];
	b32 now = JAM.now_input[key];
	b32 is_pressed = now && !was;
	b32 is_released = !now && was;
	b32 input = is_released << 2 | is_pressed << 1 | now << 0;
	elf_add_int(S,input);
	return 1;
}


static int l_cycle(elf_State *S) {
	r_flush();

	cycle_window(&JAM);
	_jr_cycle(&JAM);
	_j_cycle_globals(S);
	elf_add_int(S,true);
	return 1;
}

typedef struct GCtexture GCtexture;
struct GCtexture {
	elf_Object object;
	jam_Texture texture;
};

static int l_get_texture_size(elf_State *S) {
	GCtexture *texture_o = (GCtexture *) elf_get_object(S,0);
	if(!texture_o){
		elf_error(S,0,elf_tpf("invalid argument, is: %s", tag2s[elf_get_tag(S,0)]));
	}
	elf_Table *size_table = elf_new_table(S);
	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"x")),VALUE_INTEGER(texture_o->texture.size.x));
	elf_table_set(size_table,VALUE_STRING(elf_alloc_string(S,"y")),VALUE_INTEGER(texture_o->texture.size.y));
	elf_add_table(S,size_table);
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
		jam_Texture texture = jam_new_texture(&JAM,w,h,pixels);
		free(pixels);

		GCtexture *texture_o = (GCtexture *) elf_new_object(S,sizeof(*texture_o));
		texture_o->texture = texture;
		elf_add_object(S,(elf_Object *) texture_o);
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

// todo: if the idea is to encourage users to do sprite
// batching, or at least make that process easier and
// more intuitive, it would be best to remove the texture
// argument from here
int j_draw_sprite(elf_State *S) {
	int i = 0;

	int nargs = elf_get_num_args(S);

	jam_Texture texture = JAM.fallback_texture;
	GCtexture *texture_o = (GCtexture *) elf_get_object(S,i++);
	if(texture_o){
		texture = texture_o->texture;
	}

	r_i32 src_r;
	i = _get_rect(S, i, &src_r);
	if(src_r.size.x == 0) src_r.size.x = texture.size.x;
	if(src_r.size.y == 0) src_r.size.y = texture.size.y;

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
		// todo: pass in basis instead!
		vec2 basis_x = (vec2){cos(rotation),sin(rotation)};
		vec2 basis_y = (vec2){-basis_x.y,basis_x.x};
		r_p0 = vec2_add(vec2_mul(vec2(r_p0.x,r_p0.x),basis_x),vec2_mul(vec2(r_p0.y,r_p0.y),basis_y));
		r_p1 = vec2_add(vec2_mul(vec2(r_p1.x,r_p1.x),basis_x),vec2_mul(vec2(r_p1.y,r_p1.y),basis_y));
		r_p2 = vec2_add(vec2_mul(vec2(r_p2.x,r_p2.x),basis_x),vec2_mul(vec2(r_p2.y,r_p2.y),basis_y));
		r_p3 = vec2_add(vec2_mul(vec2(r_p3.x,r_p3.x),basis_x),vec2_mul(vec2(r_p3.y,r_p3.y),basis_y));
	}

	_jr_set_texture(texture);


	f32 inv_src_w = 1.0 / texture.size.x;
	f32 inv_src_h = 1.0 / texture.size.y;

	f32 u0 = src_r.x * inv_src_w;
	f32 v0 = src_r.y * inv_src_h;
	f32 u1 = (src_r.x + src_r.w) * inv_src_w;
	f32 v1 = (src_r.y + src_r.h) * inv_src_h;

	vec2 dst_pos = vec2(dst_r.x,dst_r.y);
	r_p0 = vec2_add(dst_pos,r_p0);
	r_p1 = vec2_add(dst_pos,r_p1);
	r_p2 = vec2_add(dst_pos,r_p2);
	r_p3 = vec2_add(dst_pos,r_p3);
	if (r_rotation) {
		f32 rotation = r_rotation;
		// todo: pass in basis instead!
		vec2 basis_x = (vec2){cos(rotation),sin(rotation)};
		vec2 basis_y = (vec2){-basis_x.y,basis_x.x};
		r_p0 = vec2_add(vec2_mul(vec2_sub(vec2(r_p0.x,r_p0.x),vec2(r_center.x,r_center.x)),basis_x),vec2_mul(vec2_sub(vec2(r_p0.y,r_p0.y),vec2(r_center.y,r_center.y)),basis_y));
		r_p1 = vec2_add(vec2_mul(vec2_sub(vec2(r_p1.x,r_p1.x),vec2(r_center.x,r_center.x)),basis_x),vec2_mul(vec2_sub(vec2(r_p1.y,r_p1.y),vec2(r_center.y,r_center.y)),basis_y));
		r_p2 = vec2_add(vec2_mul(vec2_sub(vec2(r_p2.x,r_p2.x),vec2(r_center.x,r_center.x)),basis_x),vec2_mul(vec2_sub(vec2(r_p2.y,r_p2.y),vec2(r_center.y,r_center.y)),basis_y));
		r_p3 = vec2_add(vec2_mul(vec2_sub(vec2(r_p3.x,r_p3.x),vec2(r_center.x,r_center.x)),basis_x),vec2_mul(vec2_sub(vec2(r_p3.y,r_p3.y),vec2(r_center.y,r_center.y)),basis_y));
	}
	r_p0 = vec2_add(r_offset,vec2_mul(r_scale,r_p0));
	r_p1 = vec2_add(r_offset,vec2_mul(r_scale,r_p1));
	r_p2 = vec2_add(r_offset,vec2_mul(r_scale,r_p2));
	r_p3 = vec2_add(r_offset,vec2_mul(r_scale,r_p3));

	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p0,{u0,v1},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p1,{u0,v0},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p2,{u1,v0},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p0,{u0,v1},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p2,{u1,v0},color};
	r_mem_vertices[r_num_vertices ++]=(Vertex2D){r_p3,{u1,v1},color};
	return 0;
}



int main() {
	elf_State R = {};
	elf_Module M = {};
	elf_init(&R,&M);

	elf_Table *globals = M.globals;
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.video")),VALUE_FUNCTION(j_video));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.audio")),VALUE_FUNCTION(l_audio));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.audio.play")),VALUE_FUNCTION(l_audio_play));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.cycle")),VALUE_FUNCTION(l_cycle));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.load_texture")),VALUE_FUNCTION(j_load_texture));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.get_texture_size")),VALUE_FUNCTION(l_get_texture_size));
	elf_table_set(globals,VALUE_STRING(elf_alloc_string(&R,"elf.jam.draw_sprite")),VALUE_FUNCTION(j_draw_sprite));
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

