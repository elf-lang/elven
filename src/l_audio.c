//
// lib audio
//

// #include "include/elf.h"
// #include "elements.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


#undef PlaySound


enum {
	VOICES_CAPACITY = 32,
	SOUNDS_CAPACITY = 256,
};

global ma_engine g_engine;
global ma_sound  g_voices[VOICES_CAPACITY];
global ma_sound  g_sounds[SOUNDS_CAPACITY];

void PlaySound(int id) {
	ma_sound *sound = &g_sounds[id];

	ma_sound *to_play = 0;
	i32 open_voice_slot = -1;

	for (i32 i = 0; i < VOICES_CAPACITY; i ++) {
		ma_sound *voice = & g_voices[i];
		if (voice->pDataSource) {
			if (ma_sound_at_end(voice)) {
				to_play = voice;
				break;
			}
		} else {
			open_voice_slot = i;
			break;
		}
	}

	if (to_play) {
		//
		// todo: only uninit if the sound that we found isn't
		// the same one we're trying to play
		//
		ma_sound_uninit(to_play);

		ma_result result = ma_sound_init_copy(&g_engine, sound, 0, 0, to_play);
		ASSERT(result == MA_SUCCESS);

	} else if (open_voice_slot != -1) {
		to_play = & g_voices[open_voice_slot];
		ma_result result = ma_sound_init_copy(&g_engine, sound, 0, 0, to_play);
		ASSERT(result == MA_SUCCESS);
	} else {
		TRACELOG("Too Many Sounds Playing Simultaneously");
	}

	if (to_play) {
		ma_sound_set_pitch(to_play, 1.0);
		ma_sound_start(to_play);
	}
}

static void AudioDeviceDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	//
   // todo: our own stuff please
	//
	ma_engine* pEngine = (ma_engine*) pDevice->pUserData;
	ma_engine_read_pcm_frames(pEngine, pOutput, frameCount, NULL);
}

ELF_FUNCTION(L_InitAudio) {
	ma_engine_config config = ma_engine_config_init();
	config.dataCallback = AudioDeviceDataCallback;
	config.pProcessUserData = 0;

	ma_result error = ma_engine_init(NULL, &g_engine);
	elf_push_int(S, error == MA_SUCCESS);
	return 1;
}

ELF_FUNCTION(L_LoadSound) {

	int id = elf_get_int(S, 0);
	char *name = elf_get_text(S, 1);

	ma_sound *sound = &g_sounds[id];

	ma_result error = ma_sound_init_from_file(&g_engine, name, 0, NULL, NULL, sound);
	elf_push_int(S, error == MA_SUCCESS);

	return 1;
}

ELF_FUNCTION(L_PlaySound) {
	i32 id = elf_get_int(S, 0);
	PlaySound(id);
	return 0;
}
