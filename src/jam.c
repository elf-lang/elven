

//
// todo: proper text rendering...
//
void jDrawText(jam_State *J, f32 x, f32 y, char *text) {
	// todo:
	__debugbreak();
	Color color = {}; // r_color_0;

	D_FONT *font = J->font;

	vec2i resolution = R_GetTextureInfo(J->rend, font->texture);
	f32 inv_resolution_x = 1.0 / resolution.x;
	f32 inv_resolution_y = 1.0 / resolution.y;

	R_SetTopology(J->rend, TOPO_TRIANGLES);
	R_SetTexture(J->rend, font->texture);
	while (*text) {
		i32 token = *text ++;
		i32 index = token - 32;

		stbtt_bakedchar glyph = font->glyphs[index];

		r_f32 src_r = { glyph.x0, glyph.y0, glyph.x1 - glyph.x0, glyph.y1 - glyph.y0 };
		r_f32 dst_r = { x + glyph.xoff, y - (src_r.h + glyph.yoff), src_r.w, src_r.h };

		vec2 r_p0 = { dst_r.x +       0, dst_r.y +       0 };
		vec2 r_p1 = { dst_r.x +       0, dst_r.y + dst_r.h };
		vec2 r_p2 = { dst_r.x + dst_r.w, dst_r.y + dst_r.h };
		vec2 r_p3 = { dst_r.x + dst_r.w, dst_r.y +       0 };

		if (token != ' ') {

			f32 u0 = src_r.x * inv_resolution_x;
			f32 v0 = src_r.y * inv_resolution_y;
			f32 u1 = (src_r.x + src_r.w) * inv_resolution_x;
			f32 v1 = (src_r.y + src_r.h) * inv_resolution_y;

			R_VERTEX_2D *vertices = R_QueueVertices(J->rend, 6);
			vertices[0]=(R_VERTEX_2D){r_p0,{u0,v1},color};
			vertices[1]=(R_VERTEX_2D){r_p1,{u0,v0},color};
			vertices[2]=(R_VERTEX_2D){r_p2,{u1,v0},color};
			vertices[3]=(R_VERTEX_2D){r_p0,{u0,v1},color};
			vertices[4]=(R_VERTEX_2D){r_p2,{u1,v0},color};
			vertices[5]=(R_VERTEX_2D){r_p3,{u1,v1},color};
		}

		x += glyph.xadvance;
	}
}

#if 0
static void AudioDeviceDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	//
   // todo: please remove engine crap
	//
	ma_engine* pEngine = (ma_engine*) pDevice->pUserData;
	ma_engine_read_pcm_frames(pEngine, pOutput, frameCount, NULL);


	jam_State *J = pEngine->pProcessUserData;
	f32 *sample_buffer = J->audio.sample_buffer;

	f32 *pFramesOut = pOutput;

	for (i32 i = 0; i < frameCount; i ++) {
		f32 sample = sample_buffer[J->audio.sample_buffer_read ++];
		pFramesOut[]
	}
}

//
// AUDIO STUFF
//
void InitAudioAPI(jam_State *J) {

	ma_engine_config config = ma_engine_config_init();
	config.dataCallback = AudioDeviceDataCallback;
	config.pProcessUserData = J;

	ma_result error = ma_engine_init(NULL, &J->audio.engine);
	ASSERT(error == MA_SUCCESS);
}
#endif

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

void PlaySound(jam_State *J, SoundId id) {
	ma_sound *sound = GetSoundFromId(J, id);

	ma_sound *to_play = 0;
	i32 open_voice_slot = -1;

	for (i32 i = 0; i < VOICES_CAPACITY; i ++) {

		ma_sound *voice = & J->audio.voices[i];

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

		ma_result result = ma_sound_init_copy(&J->audio.engine, sound, 0, 0, to_play);
		ASSERT(result == MA_SUCCESS);

	} else if (open_voice_slot != -1) {

		to_play = & J->audio.voices[open_voice_slot];
		ma_result result = ma_sound_init_copy(&J->audio.engine, sound, 0, 0, to_play);
		ASSERT(result == MA_SUCCESS);

	} else {
		TRACELOG("Too Many Sounds Playing Simultaneously");
	}

	if (to_play) {
		ma_sound_set_pitch(to_play, 1.0);
		ma_sound_start(to_play);
	}
}

#endif