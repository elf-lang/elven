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

struct {
	elf_State R;
	elf_Module M;
} GLOBAL elf = {{&elf.M}};

#include "lib_image.c"
#include "lib_draw.c"
#include "lib_window.c"
int main() {
	elf_init(&elf.R,&elf.M);
	lib_gfx_window_init(&elf.R);
	lib_gfx_image_init(&elf.R);
	elf_add_cfn(&elf.R,core_lib_load_file);
	elf_add_nil(&elf.R);
	elf_new_string(&elf.R,"launch.elf");
	elf_call(&elf.R,2,0);
}