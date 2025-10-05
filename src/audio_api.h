// the best audio api you'll ever see
typedef struct Sound Sound;

typedef struct  {
	f32    pitch;
	f32    volume;
} PLAY_SOUND;

#define PLAY_SOUND_DEFAULTS ((PLAY_SOUND){1.0,1.0})

int A_InitAudio();
Sound *A_LoadSoundFromFile(const char *name);

int A_PlaySound(Sound *sound, PLAY_SOUND);

int A_GetNumVoices();
void A_StopVoice(int id);
