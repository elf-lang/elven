
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IM PASS BUILDER /////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D_Init(DrawContext *ctx) {
	// Todo, allocate this sensibly!
	u32 arena_size = MEGABYTES(64);
	g_core.verts = calloc(1, arena_size);
	g_core.passes = calloc(1, arena_size);
	g_core.max_verts = arena_size / sizeof(* g_core.verts);
	g_core.max_passes = arena_size / sizeof(* g_core.passes);
}

b32 IsMemoryZero(const void *data, u32 size) {
	for (u32 i = 0; i < size; ++ i) {
		if (((u8 *) data)[i]) return false;
	}
	return true;
}

void D_CheckPassParams(R_PassParams *params) {
	if (params->viewport.w == 0 || params->viewport.h == 0) {
		TRACELOG("viewport is zero, nothing will be seen");
	}
	if (params->scissor.w == 0 || params->scissor.h == 0) {
		TRACELOG("scissor is zero, nothing will be seen");
	}
	if (params->sampler == 0) {
		TRACELOG("sampler is zero, nothing will be seen");
	}
	if (params->texture == 0) {
		TRACELOG("texture is zero, nothing will be seen");
	}
	if (params->shader == 0) {
		TRACELOG("shader is zero, nothing will be seen");
	}
	if (IsMemoryZero(&params->mixer, sizeof(params->mixer))) {
		TRACELOG("mixer is zero, nothing will be seen");
	}
	//	if (IsMemoryZero(&params->transform, sizeof(params->transform))) {
	//		TRACELOG("transform is zero, nothing will be seen");
	//	}
}

void D_EndPass(DrawContext *ctx) {
	u32 pass_length = g_core.num_verts - g_core.pass_offset;
	ASSERT(pass_length <= g_core.num_verts);

	if (pass_length) {
		ASSERT(g_core.num_passes < g_core.max_passes);
		D_CheckPassParams(&g_core.params);
		g_core.passes[g_core.num_passes].params = g_core.params;
		g_core.passes[g_core.num_passes].offset = g_core.pass_offset;
		g_core.passes[g_core.num_passes].length = pass_length;
		++ g_core.num_passes;

		// new pass
		g_core.pass_offset = g_core.num_verts;
	}
}

void D_SetSampler(DrawContext *ctx, R_Sampler sampler) {
	if (g_core.params.sampler != sampler) {
		D_EndPass(ctx);
		g_core.params.sampler = sampler;
	}
}

void D_SetBlender(DrawContext *ctx, R_Blender blender) {
	if (g_core.params.blender != blender) {
		D_EndPass(ctx);
		g_core.params.blender = blender;
	}
}

void D_SetShader(DrawContext *ctx, R_Shader shader) {
	if (g_core.params.shader != shader) {
		D_EndPass(ctx);
		g_core.params.shader = shader;
	}
}

void D_SetTopology(DrawContext *ctx, R_Topology topology) {
	if (g_core.params.topology != topology) {
		D_EndPass(ctx);
		g_core.params.topology = topology;
	}
}

void D_PushVertex(DrawContext *ctx, R_Vertex vert) {
	vec4 position = mul_matrix_v4(g_core.xform, vec4_3(vert.position, 1.0));
	vert.position.x = position.x;
	vert.position.y = position.y;
	vert.position.z = position.z;
	vert.texture_blend.x = g_core.texture_blend;
	ASSERT(g_core.num_verts < g_core.max_verts);
	g_core.verts[g_core.num_verts ++] = vert;
}

void D_SetMixer(DrawContext *ctx, Matrix transform) {
	g_core.params.mixer = transform;
}

void D_SetTexture(DrawContext *ctx, RID texture) {
	ASSERT(texture != RID_NONE);

	if (g_core.params.texture != texture) {

		D_EndPass(ctx);

		g_core.params.texture = texture;

		// unless the color transform changed...
		D_SetMixer(ctx, texture->mixer);

		vec2i resolution = texture->reso;
		g_core.texture_inv_resolution.x = 1.0 / resolution.x;
		g_core.texture_inv_resolution.y = 1.0 / resolution.y;
		g_core.region.u0 = 0;
		g_core.region.v0 = 0;
		g_core.region.u1 = 1;
		g_core.region.v1 = 1;
	}

	g_core.texture_blend = 1.f;
}

void D_SetOrthoTransform(DrawContext *ctx, vec2 reso) {
	vec2 scale = { 2.0 / reso.x, 2.0 / reso.y };
	vec2 offset = { -1.0, -1.0 };

	g_core.xform.rows[0] = (vec4){scale.x, 0, 0, offset.x};
	g_core.xform.rows[1] = (vec4){0, scale.y, 0, offset.y};
	g_core.xform.rows[2] = (vec4){0, 0, 1, 0};
	g_core.xform.rows[3] = (vec4){0, 0, 0, 1};
}


void D_SetViewport(DrawContext *ctx, rect_i32 viewport) {
	if (!RECT_MATCH(g_core.params.viewport, viewport)) {
		D_EndPass(ctx);
		g_core.params.viewport = viewport;
	}
}

void D_SetScissors(DrawContext *ctx, rect_i32 scissor) {
	if (!RECT_MATCH(g_core.params.scissor, scissor)) {
		D_EndPass(ctx);
		g_core.params.scissor = scissor;
	}
}

void D_SetOutput(DrawContext *ctx, RID output) {
	if (g_core.params.output != output) {
		D_EndPass(ctx);
		g_core.params.output = output;
		D_SetViewport(ctx, rect_i32_from_size(output->reso));
		D_SetScissors(ctx, rect_i32_from_size(output->reso));
		D_SetOrthoTransform(ctx, (vec2){output->reso.x, output->reso.y});
	}
}

RID D_GetTexture(DrawContext *ctx) {
	return g_core.params.texture;
}

RID R_GetSurface(DrawContext *ctx) {
	return g_core.params.output;
}

static void D_BeginDrawing(DrawContext *ctx) {
	g_core.num_passes = 0;
	g_core.num_verts = 0;
	g_core.pass_offset = 0;
}