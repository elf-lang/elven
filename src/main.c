#define _CRT_SECURE_NO_WARNINGS
#include "include/elf.h"

// <3
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// <3
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdlib.h>

#include "elements.h"
#include "platform.h"
#include "draw_2d.h"
#include "renderer.h"

#include "jam.h"

#include "fonts.h"

#include "jam.c"

#include "l_audio.c"
#include "lib.c"


// todo: convert command line arguments into real arguments
int main(int nargs, char **args) {

	char *name = "main.elf";

	// todo: proper parser for this!
	if (nargs >= 2) {
		name = args[1];
	}

	OS_InitPlatform();
	elf_State *inter = elf_new();

	elf_push_globals(inter);
	for (int i = 0; i < COUNTOF(g_lib); i ++) {
		elf_push_string(inter, g_lib[i].name);
		elf_push_function(inter, g_lib[i].function);
		elf_table_set(inter);
	}

	// todo: expect one return for the exit code
	elf_push_string(inter, name);
	elf_read_file(inter, -1);
	elf_exec(inter, 0, 0, false);
}
