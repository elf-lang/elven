#define _DEBUG
#define ELF_KEEPWINDOWS
// #define ELF_NOMAIN
// #include "elf\elf-web.c"
#include "elf\elf.c"
#include "timeapi.h"
#define KIT_IMPL
#include "kit\kit.h"
#include "windows.h"

typedef elf_State  elState;
typedef elf_String elString;
typedef elf_Int    elInteger;

#include "lib_image.c"
#include "lib_draw.c"
#include "lib_window.c"
int main() {
	elf_State R = {};
	elf_Module M = {};
	elf_init(&R,&M);
	lib_gfx_window_init(&R);
	lib_gfx_image_init(&R);
	elf_push_function(&R,core_lib_load_file);
	elf_push_nil(&R);
	elf_new_string(&R,"launch.elf");
	elf_call(&R,2,0);
}