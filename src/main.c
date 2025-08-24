#define _CRT_SECURE_NO_WARNINGS
#include "include/elf.h"
#include "src/platform/system.h"


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
#include "renderer.h"
#include "drawstate.h"
#include "draw_helpers.c"
#include "l_image.c"
#include "l_state.c"
#include "l_tiles.c"
#include "l_archive.c"


int main(int nargs, char **args) {

	char *name = "main.elf";

	if (nargs >= 2) {
		name = args[1];
	}

	OS_InitPlatform();
	elf_State *S = elf_new();

	// todo: to be replaced with a directory with proper function definitions
	elf_pushglobals(S);
	for (int i = 0; i < COUNTOF(l_state); i ++) {
		elf_pushtext(S, l_state[i].name);
		elf_pushfun(S, l_state[i].function);
		elf_setfield(S);
	}
	for (int i = 0; i < COUNTOF(l_tiles); i ++) {
		elf_pushtext(S, l_tiles[i].name);
		elf_pushfun(S, l_tiles[i].function);
		elf_setfield(S);
	}

	for (int i = 0; i < COUNTOF(l_arch); i ++) {
		elf_pushtext(S, l_arch[i].name);
		elf_pushfun(S, l_arch[i].function);
		elf_setfield(S);
	}

	// todo: expect one return for the exit code
	elf_pushcodefile(S, name);
	elf_pushnil(S);

	// todo: proper parser for this!
	int ncallargs = 1;
	for (int i=2; i<nargs; ++i) {
		char *arg = args[i];
		if (!strcmp(arg, "-s")){
			char asstr[256]={};
			sprintf(asstr, "\"%s\"", args[i+1]);
			arg = asstr;
			i ++;
		}
		elf_load_const_expr_from_text(S, "arg", arg);
		ncallargs ++;
	}

	int nrets = elf_call(S, ncallargs, 0);
	return 0;
}
