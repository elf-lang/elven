// todo: this is bugged!
// todo: can we stop using mini-audio?
#define WIN32_LEAN_AND_MEAN
#define MINIAUDIO_IMPLEMENTATION
#define MA_API static
#include "miniaudio.h"

#include "elements.h"

// todo: stop using mini audio's bullshit
typedef struct MSOUND {
	ma_sound sound; // must be first field!
	b32      loaded;
	// todo: load the sounds as needed!
	// char     path[256];
} MSOUND;

typedef struct MVOICE {
	ma_sound sound; // must be first field!
	b32      id;
} MVOICE;


global ma_engine g_engine;

enum {
	VOICES_CAPACITY = 64,
	SOUNDS_CAPACITY = 2048,
};

global MVOICE  g_voices[VOICES_CAPACITY];
global MSOUND  g_sounds[SOUNDS_CAPACITY];


int A_LoadSoundFromFile(int id, char *name) {
	ASSERT(id < SOUNDS_CAPACITY);

	MSOUND *msnd = &g_sounds[id];
	ma_sound *sound = (ma_sound *) msnd;
	if (msnd->loaded) {
		ma_sound_uninit(sound);
	}

	// todo: remove this please!
	wchar_t namew[1024] = {};
	MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, name, -1, namew, sizeof(namew));

	ma_result error = ma_sound_init_from_file_w(&g_engine, namew, 0, NULL, NULL, sound);
	if (error == MA_SUCCESS) {
		msnd->loaded = true;
	}
	return error == MA_SUCCESS;
}

int A_GetNumVoices() {
	return VOICES_CAPACITY;
}

int A_GetVoiceSound(int voiceid) {
	MVOICE *voice = & g_voices[voiceid];
	int soundid = -1;
	if (voice->sound.pDataSource != 0) {
		if (ma_sound_is_playing(&voice->sound)) {
			soundid = voice->id;
		}
	}
	return soundid;
}


void A_StopVoice(int id) {
	MVOICE *voice = & g_voices[id];
	ma_sound_stop(&voice->sound);
}

int A_PlaySound(int id) {
	i32 voiceid = -1;

	MSOUND *msnd = &g_sounds[id];

	if (msnd->loaded) {
		ma_sound *sound = (ma_sound *) msnd;

		for (i32 i = 0; i < VOICES_CAPACITY; i ++) {
			MVOICE *voice = & g_voices[i];

			if (!voice->sound.pDataSource) {

				ma_result error = ma_sound_init_copy(&g_engine, sound, 0, 0, &voice->sound);
				ASSERT(error == MA_SUCCESS);

				ma_sound_start(&voice->sound);

				// TRACELOG("New Sound: %i", i);
				voice->id = id;
				voiceid = i;

				break;
			} else {

				if (ma_sound_at_end(&voice->sound)) {

					if (id == voice->id) {
						ma_sound_start(&voice->sound);
						// TRACELOG("Restart Sound: %i", i);
					} else {

						ma_sound_uninit(&voice->sound);

						ma_result error = ma_sound_init_copy(&g_engine, sound, 0, 0, &voice->sound);
						ASSERT(error == MA_SUCCESS);

						ma_sound_start(&voice->sound);

						// TRACELOG("Reuse Sound: %i", i);
					}

					voice->id = id;
					voiceid = i;
					break;
				}
			}
		}
	}

	esc:
	return voiceid;
	// ma_sound_set_pitch(to_play, 1.0);
}

static void AudioDeviceDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	//
   // todo: our own stuff please
	//
	ma_engine* pEngine = (ma_engine*) pDevice->pUserData;
	ma_engine_read_pcm_frames(pEngine, pOutput, frameCount, NULL);
}

int A_InitAudio() {
	ma_engine_config config = ma_engine_config_init();
	config.dataCallback = AudioDeviceDataCallback;
	config.pProcessUserData = 0;
	ma_result error = ma_engine_init(NULL, &g_engine);
	return error == MA_SUCCESS;
}
