// todo: I think people may want things like master volume, sound groups, effects and such...
// todo: better interpolation for better pitch shifting
// todo: do streaming!
// todo: lazy loading?!
// todo: remove file io from here!?
// todo: proper clipping, I still hear glitches!
#define WIN32_LEAN_AND_MEAN

// it's not like it saves compilation time
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
// #define MA_NO_THREADING
#define MA_NO_GENERATION

// is there a version that's just the device?
#define MINIAUDIO_IMPLEMENTATION
#define MA_API static
#include "miniaudio.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

#include "base.h"
#include "audio_api.h"

enum {
	VOICES_CAPACITY = 64,
};


typedef struct {
	f32 l, r;
} Audio_Frame;


struct Sound {
	u32    sample_rate;
	u32     num_frames;
	Audio_Frame frames[];
};

// todo: maybe storing the "playing" bit in a single 64 bit integer
// could be a good idea
enum {
	STATUS_PLAYING = 1,
	STATUS_LOOPING = 2,
};

typedef struct {
	u32                    status;
	f32                    volume;
	f32                     pitch;
	struct {
		f32            play_cursor;

		u32            sample_rate;
		u32             num_frames;
		Audio_Frame        *frames;
	};
} Voice;

struct {
	ma_device   device;
	Voice       voices[VOICES_CAPACITY];
	_Atomic i32 num_voices;
} global g;


int A_GetNumVoices() {
	return COUNTOF(g.voices);
}

void A_StopVoice(int id) {
	// todo:
	g.voices[id].status &= ~STATUS_PLAYING;
}

static void Mixer(Audio_Frame *dst, int num_frames, int sample_rate);

static void AudioDeviceDataCallback2(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	Mixer((Audio_Frame*)pOutput, frameCount, pDevice->sampleRate);
}

int A_InitAudio() {
	TRACELOG("InitAudio2...");

	ma_device_config config = ma_device_config_init(ma_device_type_playback);
	config.playback.format   = ma_format_f32;
	config.playback.channels = 2;
	// todo: do we care?
	config.sampleRate        = 48000;
	config.dataCallback      = AudioDeviceDataCallback2;
	config.pUserData         = NULL;


	ma_result result = ma_device_init(NULL, &config, &g.device);
	ASSERT(result == MA_SUCCESS);

	result = ma_device_start(&g.device);
	ASSERT(result == MA_SUCCESS);

	return result == MA_SUCCESS;
}

// todo: put this somewhere!
static const char *get_file_extension(const char *p) {
	int l = strlen(p);
	const char *s = p;
	const char *e = p + l;
	while (e > s && e[-1] != '\\' && e[-1] != '/' && e[-1] != '.') e --;
	// *o = l - (e - p);
	return e;
}

#define FILETYPEDEF(_) \
_(OGG ,  "ogg") \
_(WAV ,  "wav") \
_(MP3 ,  "mp3") \
_(FLAC, "flac") \
/* end */

#define FILEDISPATCHDEF(_) \
_(WAV ,  WAV) \
_(MP3 ,  MP3) \
_(OGG , FLAC) \
_(FLAC, FLAC) \
/* end */

typedef enum {
	FILETYPE_UNKNOWN = 0,
#define FILETYPEENUM(NAME,EXTENSION) FILETYPE_##NAME,
	FILETYPEDEF(FILETYPEENUM)
#undef FILETYPEENUM
} FileType;



// if only this mattered
static inline FileType get_file_type(const char *name) {
	const char *extension = get_file_extension(name);
	switch (*extension) {
		case 'm': {
			if (!strcmp(extension, "mp3")) return FILETYPE_MP3;
		} break;
		case 'f': {
			if (!strcmp(extension, "flac")) return FILETYPE_FLAC;
		} break;
		case 'o': {
			if (!strcmp(extension, "ogg")) return FILETYPE_OGG;
		} break;
		case 'w': {
			if (!strcmp(extension, "wav")) return FILETYPE_WAV;
		} break;
	}
	return FILETYPE_UNKNOWN;
}

static void _check_channels(const char *name, int channels) {
	if (channels == 1) return;
	if (channels == 2) return;
	TRACELOG("warning: unsupported channel count: %i, file: %s", channels, name);
}

static Sound *NewSound(u32 num_frames, u32 sample_rate) {
	u32 total_frames = num_frames + 4 + 15 & ~ 15;
	Sound *sound = malloc(sizeof(*sound) + sizeof(*sound->frames) * total_frames);
	sound->num_frames = num_frames;
	sound->sample_rate = sample_rate;
	memset(sound->frames + num_frames, 0, total_frames - num_frames);
	return sound;
}

static Sound *LoadSound_WAV(const char *name) {
	drwav decoder;
	if (!drwav_init_file(&decoder, name, NULL)) {
		return 0;
	}

	_check_channels(name, decoder.channels);

	Sound *ref = NewSound(decoder.totalPCMFrameCount, decoder.sampleRate);

	f32 *buf = (f32 *) ref->frames;

	if (decoder.channels != 2) {
		buf = malloc(sizeof(*buf) * decoder.channels * decoder.totalPCMFrameCount);
	}

	drwav_read_pcm_frames_f32(&decoder, decoder.totalPCMFrameCount, buf);

	if (decoder.channels == 1) {
		for (int i = 0; i < decoder.totalPCMFrameCount; i ++) {
			ref->frames[i].l = buf[i];
			ref->frames[i].r = buf[i];
		}
	}

	if (buf != (f32 *) ref->frames) {
		free(buf);
	}

	drwav_uninit(&decoder);
	return ref;
}

static Sound *LoadSound_FLAC(const char *name) {
	drflac *decoder = drflac_open_file(name, NULL);
	if (!decoder) {
		return 0;
	}
	_check_channels(name, decoder->channels);
	Sound *snd = NewSound(decoder->totalPCMFrameCount, decoder->sampleRate);
	drflac_read_pcm_frames_f32(decoder, decoder->totalPCMFrameCount, (float *) snd->frames);
	drflac_free(decoder, NULL);
	return snd;
}

static Sound *LoadSound_MP3(const char *name) {
	drmp3 decoder;
	if (!drmp3_init_file(&decoder, name, NULL)) {
		return 0;
	}
	_check_channels(name, decoder.channels);
	Sound *snd = NewSound(decoder.totalPCMFrameCount, decoder.sampleRate);
	drmp3_read_pcm_frames_f32(&decoder, decoder.totalPCMFrameCount, (float *) snd->frames);
	drmp3_uninit(&decoder);
	return snd;
}

Sound *A_LoadSoundFromFile(const char *name) {
	FileType type = get_file_type(name);
	switch (type) {
#define FILEDISPATCH(NAME,DISPATCH) case FILETYPE_##NAME: return LoadSound_##DISPATCH(name);
		FILEDISPATCHDEF(FILEDISPATCH)
#undef FILEDISPATCH
		default: ;
	}
	return 0;
}

int A_PlaySound(Sound *s, PLAY_SOUND play) {
	Voice *v;
	for (v = g.voices; v < g.voices + VOICES_CAPACITY; ++ v)
	{
		if (~v->status & STATUS_PLAYING)
		{
			v->volume       = play.volume;
			v->pitch        = play.pitch;

			v->sample_rate  = s->sample_rate;
			v->play_cursor  = 0;
			v->num_frames   = s->num_frames;
			v->frames       = s->frames;
			// set status last
			v->status       = STATUS_PLAYING;

			g.num_voices += 1;

			return v - g.voices;
		}
	}
	return -1;
}

static inline f32 clip(f32 x) {
	if (x > 1.0f) x = 1.0f + tanhf(x - 1.0f);
	else if (x < -1.0f) x = -1.0f + tanhf(x + 1.0f);
	return CLAMP(x, -1.0f, 1.0f);
}

static void Mixer(Audio_Frame *dst, int num_frames, int sample_rate) {

	Voice *v;
	for (v = g.voices; v < g.voices + VOICES_CAPACITY; ++ v) {
		if (v->status & STATUS_PLAYING) {

			f32 advance = v->sample_rate / (f32) sample_rate * v->pitch;

			f32 volume = v->volume;
			Audio_Frame *src = v->frames;

			i32 remaining_frames = v->num_frames - (int) v->play_cursor;

			i32 max_frames = MIN(remaining_frames, num_frames);
			for (int i = 0; i < max_frames; ++ i) {
				int read_cursor = (int) v->play_cursor;

				f32 l0 = src[read_cursor + 0].l;
				f32 r0 = src[read_cursor + 0].r;

				f32 l1 = src[read_cursor + 1].l;
				f32 r1 = src[read_cursor + 1].r;

				f32 t = v->play_cursor - read_cursor;

				f32 l = l0 + (l1 - l0) * t;
				f32 r = r0 + (r1 - r0) * t;

				dst[i].l += l * volume;
				dst[i].r += r * volume;

				v->play_cursor += advance;
			}

			if ((int) v->play_cursor + 1 >= v->num_frames - 1) {
				v->status &= ~STATUS_PLAYING;
				g.num_voices -= 1;
			}
		}
	}

	for (int i=0; i<num_frames; ++i){
		dst[i].l=clip(dst[i].l);
		dst[i].r=clip(dst[i].r);
	}
}



