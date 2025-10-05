
#include "audio_api.h"


struct {
	PLAY_SOUND play_params;
} g_audio;

ELF_FUNCTION(L_InitAudio) {
	int error = A_InitAudio();
	elf_pushint(S, error);
	g_audio.play_params = PLAY_SOUND_DEFAULTS;
	return 1;
}

ELF_FUNCTION(L_LoadSound) {
	const char *name = elf_loadtext(S, 1);
	Sound *sound = A_LoadSoundFromFile(name);
	if (sound) {
		elf_pushsys(S, (elf_Handle) sound);
	}
	else {
		elf_pushnil(S);
	}
	return 1;
}

ELF_FUNCTION(L_SetPitch) {
	f32 pitch = elf_loadnum(S, 1);
	g_audio.play_params.pitch = pitch;
	return 0;
}

ELF_FUNCTION(L_SetVolume) {
	f32 volume = elf_loadnum(S, 1);
	g_audio.play_params.volume = volume;
	return 0;
}

ELF_FUNCTION(L_PlaySound) {
	Sound *sound = (Sound *) elf_loadsys(S, 1);

	int voice = A_PlaySound(sound, g_audio.play_params);
	elf_pushint(S, voice);

	g_audio.play_params = PLAY_SOUND_DEFAULTS;
	return 1;
}

ELF_FUNCTION(L_GetNumVoices) {
	int n = A_GetNumVoices();
	elf_pushint(S, n);
	return 1;
}

ELF_FUNCTION(L_StopVoice) {
	int voice = elf_loadint(S, 1);
	A_StopVoice(voice);
	return 0;
}

elf_Binding l_audio[] = {
	{ "InitAudio"               ,    L_InitAudio        },
	{ "LoadSound"               ,    L_LoadSound        },
	{ "PlaySound"               ,    L_PlaySound        },
	{ "GetNumVoices"            ,    L_GetNumVoices     },
	{ "StopVoice"               ,    L_StopVoice        },
	{ "SetPitch"                ,    L_SetPitch         },
	{ "SetVolume"               ,    L_SetVolume        },
};