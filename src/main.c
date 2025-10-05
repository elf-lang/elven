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


#include "src/dynamic_array.c"


#include "platform_api.h"
#include "renderer_api.h"

#include "fonts.h"

#include "lib_helpers.c"


#include "ttf.h"
#include "ttf_stb.c"


#include "drawstate.c"

#include "l_state.c"

#include "fonts.c"
#include "tile_texture.c"
#include "l_image.c"
#include "l_archive.c"
#include "l_font.c"
#include "l_audio.c"
#include "l_tile_texture.c"





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

	for (int i = 0; i < COUNTOF(l_arch); i ++) {
		elf_pushtext(S, l_arch[i].name);
		elf_pushfun(S, l_arch[i].function);
		elf_setfield(S);
	}

	for (int i = 0; i < COUNTOF(l_font); i ++) {
		elf_pushtext(S, l_font[i].name);
		elf_pushfun(S, l_font[i].function);
		elf_setfield(S);
	}

	for (int i = 0; i < COUNTOF(l_image); i ++) {
		elf_pushtext(S, l_image[i].name);
		elf_pushfun(S, l_image[i].function);
		elf_setfield(S);
	}


	for (int i = 0; i < COUNTOF(l_audio); i ++) {
		elf_pushtext(S, l_audio[i].name);
		elf_pushfun(S, l_audio[i].function);
		elf_setfield(S);
	}

	for (int i = 0; i < COUNTOF(l_tile_texture); i ++) {
		elf_pushtext(S, l_tile_texture[i].name);
		elf_pushfun(S, l_tile_texture[i].function);
		elf_setfield(S);
	}

	elf_pushcodefile(S, name, 0);
	elf_pushnil(S);

	// todo: proper!
	int ncallargs = 1;
	for (int i=2; i<nargs; ++i) {
		char *arg = args[i];
		if (!strcmp(arg, "-s")){
			char asstr[256]={};
			sprintf(asstr, "\"%s\"", args[i+1]);
			arg = asstr;
			i ++;
		}
		elf_pushconstexpr(S, "arg", arg);
		ncallargs ++;
	}

	// todo: expect one return for the exit code
	int nrets = elf_call(S, ncallargs, 0);
	return 0;
}
