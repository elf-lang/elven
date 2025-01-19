#define _DEBUG
#include "elf\elf.c"
#include "timeapi.h"
#define KIT_IMPL
#include "kit\kit.h"
#include "lib_image.c"
#include "lib_draw.c"
#include "lib_window.c"
#include "lib_audio.c"

#include "package.c"



int main() {

#if 0
	kit_Context *test = kit_create("test",320,240,KIT_SCALE2X);
	double prev = 0;
	double time = 0;
	kit_Image *src = kit_load_image_file("C:\\work\\MicroRJ\\platformer\\data\\images\\entities\\player.png");
	kit_Image *dst = test->screen;
	f32x2 o = (f32x2){160,120};
	while (kit_step(test,&prev)) {
		kit_clear(test,KIT_BLACK);

		if(kit_key_down(test,'A')){
			time -= 0.01;
			// o.x -= 1;
		}
		if(kit_key_down(test,'D')){
			time += 0.01;
			// o.x += 1;
		}

		for (int i = 0; i < 1000; i ++) {
			_rect(test->screen
			,(kit_Rect){o.x,o.y,src->w*2,src->h*2}
			,src
			,(kit_Rect){0,0,src->w,src->h}
			,(f32x2){0,0}
			,time);
		}

		int _text_y = 0;
#define _text(text,...) kit_draw_text(test, KIT_WHITE, elf_tpf(text,__VA_ARGS__), 0,_text_y), _text_y += 15;
		_text("fps: %.2f",1.0/prev);
	}
#endif
	elf_State R = {};
	elf_Module M = {};
	elf_init(&R,&M);
	lib_window_include(&R);
	lib_image_include(&R);
	elf_add_lib(&R,"elf.package",lib_package,_countof(lib_package));
	elf_add_lib(&R,"elf.audio",lib_audio,_countof(lib_audio));
	elf_push_proc(&R,core_lib_load_file);
	elf_push_nil(&R);
	elf_new_string(&R,"launch.elf");
	elf_call(&R,2,0);
}