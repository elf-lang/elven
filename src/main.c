
#include "include/elf.h"
#include "src/core/r_core.h"
#include "src/internal_utils.h"
#include "src/r_auxilary.h"


// <3
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// <3
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// <3
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// <3
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


#include <stdlib.h>

#include "elements.h"
#include "platform.h"
#include "draw.h"
#include "renderer.h"

#include "jam.h"

#include "fonts.c"
#include "draw_2d.h"

#include "jam.c"

OS_State g_os;

#include "lib.c"


int main() {
	OS_InitPlatform(&g_os);

	jam_State jam = {};
	elf_init(&jam.R);


	elf_Table *globals = jam.R.globals;
	for (int i = 0; i < COUNTOF(g_lib); i ++) {
		elf_String *name = elf_alloc_string(&jam.R, g_lib[i].name);
		elf_table_set(globals, VALUE_STRING(name), VALUE_FUNCTION(g_lib[i].fn));
	}

	elf_exec_file(&jam.R, "launch.elf", 0, 0);
}
