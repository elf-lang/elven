

Vertex2D     *r_mem_vertices;
i32           r_num_vertices;
i32           r_max_vertices;
vec2      	  r_scale;
vec2      	  r_offset;
f32       	  r_rotation;
vec2      	  r_center;
Color         r_color;

static void rDrawVertices(jam_State *J, SubmissionQueueToken offset, i32 num_vertices);
static void rFlushVertices(jam_State *J);

static void rInstallShader(jam_State *J, ShaderId id, char *entry, char *contents) {

	enum {
		SHADER_COMPILE_FLAGS = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR|D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION|D3DCOMPILE_WARNINGS_ARE_ERRORS,
	};

	ID3DBlob *bytecode_b = NULL;
	ID3DBlob *messages_b = NULL;

	ID3D11PixelShader *shader = NULL;

	if (SUCCEEDED(D3DCompile(contents, strlen(contents)
	,		entry, 0, 0, entry, "ps_5_0"
	,		SHADER_COMPILE_FLAGS, 0, &bytecode_b, &messages_b)))
	{
		ID3D11Device_CreatePixelShader(J->device
		, bytecode_b->lpVtbl->GetBufferPointer(bytecode_b)
		, bytecode_b->lpVtbl->GetBufferSize(bytecode_b)
		, NULL, &shader);
	}
	if (messages_b) {
		TRACELOG("%s",(char*) messages_b->lpVtbl->GetBufferPointer(messages_b));
	}
	if (bytecode_b) bytecode_b->lpVtbl->Release(bytecode_b);
	if (messages_b) messages_b->lpVtbl->Release(messages_b);

	J->shaders[id.index] = shader;
}

//
// todo: create a function like this, but it is internally implemented
// in the renderer... So we give the renderer the entire draw state,
// and the vertexoffsetinsubmission buffer.
//
JAPI void rDrawVertices(jam_State *J, SubmissionQueueToken offset, i32 number) {

	JDrawState prev = J->draw_prev;
	JDrawState prox = J->draw_prox;
	J->draw_prev = prox;

	ASSERT(number != 0);

	switch (prox.topology) {
		case MODE_TRIANGLES: { ASSERT(number % 3 == 0); } break;
		case MODE_LINES:     { ASSERT(number % 2 == 0); } break;
		default: { ASSERT(!"SHMUCK"); } break;
	}


	ASSERT(prox.topology != MODE_NONE);
	ASSERT(prox.texture != TEXTURE_NONE);


	if (prev.topology != prox.topology) {
		ID3D11DeviceContext_IASetPrimitiveTopology(J->context, prox.topology);
	}

	if (prev.shader.index != prox.shader.index) {
		ID3D11PixelShader *shader = J->shaders[prox.shader.index];
		ID3D11DeviceContext_PSSetShader(J->context, shader, 0, 0);
	}

	if (prev.texture != prox.texture) {
		rTextureStruct *texture = & J->textures[prox.texture];
		ID3D11ShaderResourceView *shader_resource_view = texture->shader_resource_view;
		ID3D11SamplerState *sampler = texture->sampler;
		ID3D11DeviceContext_PSSetShaderResources(J->context, 0, 1, &shader_resource_view);
		ID3D11DeviceContext_PSSetSamplers(J->context, 0, 1, &sampler);
	}

	ID3D11DeviceContext_Draw(J->context, number, offset.offset);
}


JAPI void rSetShader(jam_State *J, ShaderId shader) {
	if (J->draw_prox.shader.index != shader.index) {
		rFlushVertices(J);
		J->draw_prox.shader = shader;
	}
}

JAPI void rSetTopology(jam_State *J, i32 topology) {
	if (J->draw_prox.topology != topology) {
		rFlushVertices(J);
		J->draw_prox.topology = topology;
	}
}

JAPI void rSetTexture(jam_State *J, TextureId id) {
	if (J->draw_prox.texture != id) {
		rFlushVertices(J);
		J->draw_prox.texture = id;
	}
}

JAPI void rFlushVertices(jam_State *J) {
	if (r_num_vertices != 0) {
		SubmissionQueueToken offset;
		offset = rSubmitVertices(J, r_mem_vertices, r_num_vertices);

		rDrawVertices(J, offset, r_num_vertices);
		r_num_vertices = 0;
	}
}

static inline Vertex2D *rQueueVertices(jam_State *J, i32 number) {
	if (r_num_vertices + number > r_max_vertices) {
		rFlushVertices(J);
	}
	Vertex2D *vertices = r_mem_vertices + r_num_vertices;
	r_num_vertices += number;
	return vertices;
}

void SolidFill(jam_State *J) {
	rSetTexture(J, TEXTURE_DEFAULT);
}

void jClear(jam_State *J, Color color) {
	f32 inv = 1.0 / 255.0;
	ID3D11RenderTargetView *render_target_view = J->base_render_target_view;
	f32 fcolor[4] = { color.r * inv, color.g * inv, color.b * inv, color.a * inv };
	ID3D11DeviceContext_ClearRenderTargetView(J->context, render_target_view, fcolor);
}

void jDrawRectangle(jam_State *J, f32 x, f32 y, f32 w, f32 h) {

	rSetTopology(J, MODE_TRIANGLES);

	Color color = r_color;
	vec2 r_p0 = { x + 0, y + 0 };
	vec2 r_p1 = { x + 0, y + h };
	vec2 r_p2 = { x + w, y + h };
	vec2 r_p3 = { x + w, y + 0 };

	//
	// todo: get this from the currently bound sampling window,
	// also, whenever this is set, allow for flipping of the
	// sampling region too.
	//
	rTextureStruct *texture = & J->textures[J->draw_prox.texture];
	f32 inv_resolution_x = 1.0 / texture->resolution.x;
	f32 inv_resolution_y = 1.0 / texture->resolution.y;

	r_i32 src_r = {0, 0, 1, 1};
	f32 u0 = src_r.x * inv_resolution_x;
	f32 v0 = src_r.y * inv_resolution_y;
	f32 u1 = (src_r.x + src_r.w) * inv_resolution_x;
	f32 v1 = (src_r.y + src_r.h) * inv_resolution_y;

	Vertex2D *vertices = rQueueVertices(J, 6);
	vertices[0]=(Vertex2D){r_p0,{u0,v1},color};
	vertices[1]=(Vertex2D){r_p1,{u0,v0},color};
	vertices[2]=(Vertex2D){r_p2,{u1,v0},color};
	vertices[3]=(Vertex2D){r_p0,{u0,v1},color};
	vertices[4]=(Vertex2D){r_p2,{u1,v0},color};
	vertices[5]=(Vertex2D){r_p3,{u1,v1},color};
}

void DrawCircle(jam_State *J, f32 x, f32 y, f32 r, f32 v) {

	rSetTopology(J, MODE_TRIANGLES);

	// todo: how do you actually make this proper...
	f32 circumference = TAU * r;
	i32 num_triangles = circumference / v;

	Color color = r_color;

	Vertex2D *vertices = rQueueVertices(J, num_triangles * 3);

	for (i32 i = 0; i < num_triangles; i ++) {
		i32 t = i * 3;
		vertices[t + 0] = (Vertex2D){{x,y},{0,0},color};

		f32 a = ((i + 0.0) / (f32) num_triangles) * TAU;
		f32 ax = x + cos(a) * r;
		f32 ay = y + sin(a) * r;
		vertices[t + 1] = (Vertex2D){{ax,ay},{0,1},color};

		f32 b = ((i + 1.0) / (f32) num_triangles) * TAU;
		f32 bx = x + cos(b) * r;
		f32 by = y + sin(b) * r;
		vertices[t + 2] = (Vertex2D){{bx,by},{1,1},color};
	}
}

void DrawLine(jam_State *J, f32 x0, f32 y0, f32 x1, f32 y1) {

	rSetTexture(J, TEXTURE_DEFAULT);
	rSetTopology(J, MODE_LINES);

	Color color = r_color;

	Vertex2D *vertices = rQueueVertices(J, 2);
	vertices[0] = (Vertex2D){{x0,y0},{0,0},color};
	vertices[1] = (Vertex2D){{x1,y1},{1,1},color};
}

void jDrawTriangle(jam_State *J, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2) {
	rSetTopology(J, MODE_TRIANGLES);

	Color color = r_color;
	Vertex2D *vertices = rQueueVertices(J, 3);
	vertices[0] = (Vertex2D){{x0,y0},{0,0},color};
	vertices[1] = (Vertex2D){{x1,y1},{1,1},color};
	vertices[2] = (Vertex2D){{x2,y2},{1,1},color};
}

//
// todo: proper text rendering...
//
void jDrawText(jam_State *J, f32 x, f32 y, char *text) {
	Color color = r_color;

	JFont *font = J->font;
	rTextureStruct *texture = & J->textures[font->texture];
	f32 inv_resolution_x = 1.0 / texture->resolution.x;
	f32 inv_resolution_y = 1.0 / texture->resolution.y;

	rSetTopology(J, MODE_TRIANGLES);
	rSetTexture(J, font->texture);
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

			Vertex2D *vertices = rQueueVertices(J, 6);
			vertices[0]=(Vertex2D){r_p0,{u0,v1},color};
			vertices[1]=(Vertex2D){r_p1,{u0,v0},color};
			vertices[2]=(Vertex2D){r_p2,{u1,v0},color};
			vertices[3]=(Vertex2D){r_p0,{u0,v1},color};
			vertices[4]=(Vertex2D){r_p2,{u1,v0},color};
			vertices[5]=(Vertex2D){r_p3,{u1,v1},color};
		}

		x += glyph.xadvance;
	}
}

//
// AUDIO STUFF
//
void InitAudioAPI(jam_State *J) {
	ma_result error = ma_engine_init(NULL, &J->audio.engine);
	ASSERT(error == MA_SUCCESS);
}


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

SoundId NewSoundId(jam_State *J) {
	i32 index = J->audio.sounds_index ++;
	return (SoundId) { index };
}

void PlaySound(jam_State *J, SoundId id) {
	ma_sound *sound = GetSoundFromId(J, id);

	ma_sound *to_play = 0;
	i32 open_voice_slot = -1;

	for (i32 i = 0; i < VOICES_CAPACITY; i ++) {

		ma_sound *voice = & J->audio.voices[i];

		if (voice) {
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