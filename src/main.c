#include "include/elf.h"
#include "src/core/r_core.h"

// <3
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// <3
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdlib.h>

#include "elements.h"
#include "platform.h"
#include "renderer.h"

#include "jam.h"

#include "draw_2d.h"
#include "fonts.h"

#include "jam.c"

OS_State g_os;

#include "l_audio.c"
#include "lib.c"


int main() {
	OS_InitPlatform(&g_os);

	jam_State jam = {};
	elf_init(&jam.R);
	elf_State *S = & jam.R;

	// elf_gc_state(&jam.R, ELF_GC_PAUSED);
	elf_push_globals(S);
	for (int i = 0; i < COUNTOF(g_lib); i ++) {
		elf_push_string(S, g_lib[i].name);
		elf_push_function(S, g_lib[i].function);
		elf_table_set(S);
	}

	elf_exec_file(&jam.R, "launch.elf", 0, 0);
}
