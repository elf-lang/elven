#define _DEBUG
#include "elf\elf.c"
#include "timeapi.h"
#define KIT_IMPL
#include "kit\kit.h"


#include "raster.c"
#include "lib_image.c"
#include "lib_draw.c"
#include "lib_window.c"
#include "lib_audio.c"

#include "package.c"



int main() {
	// raster_demo();
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