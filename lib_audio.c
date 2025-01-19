#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

int lib_audio_init(elf_State *S);
int lib_audio_play(elf_State *S);

static elf_CBinding lib_audio[] = {
	"init", lib_audio_init,
	"play", lib_audio_play,
};

static ma_engine engine;
int lib_audio_init(elf_State *S) {
	ma_result error;
	error = ma_engine_init(NULL,&engine);
	elf_push_int(S,error==MA_SUCCESS);
	return 1;
}

int lib_audio_play(elf_State *S) {
	char *name = elf_get_text(S,0);
	ma_engine_play_sound(&engine,name, NULL);
	return 0;
}