#define _CRT_SECURE_NO_WARNINGS
#include "include/elf.h"


// <3
// #define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// <3
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdlib.h>

#include "elements.h"
#include "platform.h"
#include "draw_2d.h"
#include "renderer.h"
#include "audio.h"
#include "baked_fonts.h"

#include "lib.c"


// todo: convert command line arguments into real arguments
int main(int nargs, char **args) {

	char *name = "main.elf";

	// todo: proper parser for this!
	if (nargs >= 2) {
		name = args[1];
	}

	OS_InitPlatform();
	elf_State *S = elf_new();

	// todo: to be replaced with a directory with proper function definitions
	elf_pullglobals(S);
	for (int i = 0; i < COUNTOF(g_lib); i ++) {
		elf_pushtext(S, g_lib[i].name);
		elf_pushfun(S, g_lib[i].function);
		elf_setfield(S);
	}

	// todo: expect one return for the exit code
	elf_pushcodefile(S, name);
	elf_pushnil(S);

	for (int i=2; i<nargs; ++i) {
		// elf_pushvalueastext(S, args[i]);
	}

	int nrets = elf_call(S, 1, 0);
	return 0;
}
