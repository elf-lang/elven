#define _DEBUG
#include "elf.c"

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
#include "lib.h"
#include "jam.c"


//
// Sound Bindings
//
static int L_InitAudio(elf_State *S) {
	jam_State *J = (jam_State *) S;
	InitAudioAPI(J);
	return 0;
}

static int L_LoadSound(elf_State *S) {
	jam_State *J = (jam_State *) S;

	char *name = elf_get_text(S, 0);
	SoundId id = NewSoundId(J);
	LoadSoundFile(J, id, name);
	return 1;
}

static int L_PlaySound(elf_State *S) {
	jam_State *J = (jam_State *) S;

	i32 id = elf_get_int(S, 0);
	PlaySound(J, (SoundId){ id });
	return 0;
}

static int L_InitWindow(elf_State *S) {
	jam_State *J = (jam_State *) S;

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

static int L_Button(elf_State *S) {
	i32 key = elf_get_int(S,0);
	elf_add_int(S, os.keyboard[key].u);
	return 1;
}

static int L_GetCursorX(elf_State *S) {
	jam_State *J = (jam_State *) S;
	elf_add_num(S, os.mouse_xy.x / (f32) J->window_dimensions.x);
	return 1;
}

static int L_GetCursorY(elf_State *S) {
	jam_State *J = (jam_State *) S;
	elf_add_num(S, 1 - os.mouse_xy.y / (f32) J->window_dimensions.y);
	return 1;
}

static int L_PollWindow(elf_State *S) {
	jam_State *J = (jam_State *) S;

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
	jam_State *J = (jam_State *) S;

	Color color = {0, 0, 0, 0};

	if (elf_get_num_args(S) > 2) {
		color.r = elf_get_int(S, 0);
		color.g = elf_get_int(S, 1);
		color.b = elf_get_int(S, 2);
		if (elf_get_num_args(S) > 3) {
			color.a = elf_get_int(S, 3);
		}
	}

	jClear(J, color);
	return 0;
}

static int L_DrawTriangle(elf_State *S) {
	jam_State *J = (jam_State *) S;

	f32 x0 = elf_get_num(S, 0);
	f32 y0 = elf_get_num(S, 1);

	f32 x1 = elf_get_num(S, 2);
	f32 y1 = elf_get_num(S, 3);

	f32 x2 = elf_get_num(S, 4);
	f32 y2 = elf_get_num(S, 5);

	jDrawTriangle(J, x0, y0, x1, y1, x2, y2);
	return 0;
}

static int L_LoadFont(elf_State *S) {
	jam_State *J = (jam_State *) S;

	char *name = elf_get_text(S, 0);
	JFont *font = LoadFont(J, name);
	J->font = font;
	return 0;
}


static int L_SolidFill(elf_State *S) {
	jam_State *J = (jam_State *) S;
	SolidFill(J);
	return 0;
}

static int L_DrawLine(elf_State *S) {
	jam_State *J = (jam_State *) S;

	f32 x0 = elf_get_num(S, 0);
	f32 y0 = elf_get_num(S, 1);
	f32 x1 = elf_get_num(S, 2);
	f32 y1 = elf_get_num(S, 3);

	DrawLine(J, x0, y0, x1, y1);
	return 0;
}

static int L_DrawCircle(elf_State *S) {
	jam_State *J = (jam_State *) S;

	f32 x = elf_get_num(S, 0);
	f32 y = elf_get_num(S, 1);
	f32 r = elf_get_num(S, 2);
	f32 v = elf_get_num(S, 3);

	DrawCircle(J, x, y, r, v);
	return 0;
}

static int L_DrawRectangle(elf_State *S) {
	jam_State *J = (jam_State *) S;

	f32 x = elf_get_num(S, 0);
	f32 y = elf_get_num(S, 1);
	f32 w = elf_get_num(S, 2);
	f32 h = elf_get_num(S, 3);

	jDrawRectangle(J, x, y, w, h);
	return 0;
}


static int L_DrawText(elf_State *S) {
	jam_State *J = (jam_State *) S;

	f32 x = elf_get_num(S, 0);
	f32 y = elf_get_num(S, 1);
	char *text = elf_get_text(S, 2);

	jDrawText(J, x, y, text);
	return 0;
}


int main() {
	jam_State jam = {};
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
		{ "InitWindow"    , L_InitWindow },
		{ "PollWindow"    , L_PollWindow },
		// { "LoadTexture", L_LoadTexture },
		{ "LoadFont"      , L_LoadFont },
		{ "Button"        , L_Button },
		{ "Clear"         , L_Clear },
		{ "GetCursorX"    , L_GetCursorX },
		{ "GetCursorY"    , L_GetCursorY },
		{ "SetScale"      , L_SetScale },
		{ "SetOffset"     , L_SetOffset },
		{ "SetRotation"   , L_SetRotation },
		{ "SetCenter"     , L_SetCenter },
		{ "SetColor"      , L_SetColor },
		{ "DrawText"      , L_DrawText },
		{ "SolidFill"     , L_SolidFill },
		{ "jDrawRectangle", L_DrawRectangle },
		{ "DrawTriangle"  , L_DrawTriangle },
		{ "DrawLine"      , L_DrawLine },
		{ "DrawCircle"    , L_DrawCircle },
		{ "InitAudio"     , L_InitAudio },
		{ "LoadSound"     , L_LoadSound },
		{ "PlaySound"     , L_PlaySound },
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
// 	jam_State *J = (jam_State *) S;

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
//		jam_State *J = (jam_State *) S;
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
