#if 0

inline ma_sound *GetSoundFromId(jam_State *J, SoundId id) {
	ASSERT(id.index >= 0 && id.index <= SOUNDS_CAPACITY);
	return & J->audio.sounds[id.index];
}


void LoadSoundFile(jam_State *J, SoundId id, char *name) {
	ma_sound *sound = GetSoundFromId(J, id);
	ma_result result = ma_sound_init_from_file(&J->audio.engine, name, 0, NULL, NULL, sound);
	if (result != MA_SUCCESS) {
		TRACELOG("Failed To Load Sound");
	}
}

// todo: proper resource allocation!
SoundId NewSoundId(jam_State *J) {
	return (SoundId) {};
}



#endif