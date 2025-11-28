
TTF_Font *D_GetFont() { return g_core.font; }


TTF_Font *TT_FontFromElf(elf_State *S, u32 i) {
	if (elf_loadtype(S, i) == ELF_TNIL) {
		return 0;
	}
	TTF_Font *result = (TTF_Font *) elf_loadsys(S, i);
	return result;
}

ELF_FUNCTION(L_SetFont)
{
	TTF_Font *font = TT_FontFromElf(S, 1);
	g_core.font = font;
	return 0;
}



ELF_FUNCTION(L_LoadFont) {
	// todo: check file extension
	const char *name = elf_loadtext(S, 1);

	FILE_HANDLE file = sys_open_file(name, SYS_OPEN_READ, SYS_OPEN_EXISTING);
	if (file != ELF_HINVALID) {
		int size = sys_size_file(file);

		void *data = malloc(size);
		sys_read_file(file, data, size);

		TTF_Font *font = ttf_load(data);
		elf_pushsys(S, (elf_Handle) font);

		sys_close_file(file);
	}
	else {
		elf_pushnil(S);
	}
	return 1;
}

ELF_FUNCTION(L_MeasureText) {
	const char *text = elf_loadtext(S, 1);

	// f32 w = MeasureText(D_GetFont(), text);
	// elf_pushnum(S, w);
	return 1;
}

ELF_FUNCTION(L_DrawText) {
	TTF_Font *font = D_GetFont();

	if (!font) {
		return 0;
	}

	i32 nextarg = 1;
	f32 x = elf_loadnum(S, nextarg ++);
	f32 y = elf_loadnum(S, nextarg ++);

	int size = 0;

	String string;
	string.text = elf_loadtextl(S, nextarg ++, &size);
	string.size = size;

	i32 height = elf_loadint(S, nextarg ++);


	STACK_FRAME(g_core.main_stack) {
		F_Run fr = F_BuildTextRun(g_core.f_cache, g_core.main_stack, font, height, string);

		D_SetTopology(&g_core.ctx, R_TOPO_TRIANGLES);

		F_Pass *pass;
		for (pass = fr.passes; pass; pass = pass->prox) {
			RID texture = pass->page->user;
			if (!texture) {
				texture = r_new_texture_r_u8_from_image(g_core.rend, R_USAGE_PATTERN_RARE_UPDATES, pass->page->atlas);
				texture->mixer = MIXER_RRRR;
				texture->label = "page texture";
				texture->filter = R_SAMPLER_POINT;
				texture->shader = R_SHADER_SIMPLE;

				pass->page->user = texture;
				// Not dirty because we just created it ...
				pass->page->dirty = 0;
			}
			texture->dirty = pass->page->dirty;

			D_SetTexture(& g_core.ctx, texture);
			for (int i = 0; i < pass->nquads; i ++) {
				rect_f32 dst = pass->quads[i].dst;
				vec3 r_p0 = { x + dst.x + dst.w * 0, y + dst.y + dst.h * 0, 0 };
				vec3 r_p1 = { x + dst.x + dst.w * 0, y + dst.y + dst.h * 1, 0 };
				vec3 r_p2 = { x + dst.x + dst.w * 1, y + dst.y + dst.h * 1, 0 };
				vec3 r_p3 = { x + dst.x + dst.w * 1, y + dst.y + dst.h * 0, 0 };
				f32 u0 = pass->quads[i].src.u0;
				f32 v0 = pass->quads[i].src.v0;
				f32 u1 = pass->quads[i].src.u1;
				f32 v1 = pass->quads[i].src.v1;
				D_PushVertex(& g_core.ctx, (R_Vertex) { r_p0, { u0, v1 }, g_core.color_0 });
				D_PushVertex(& g_core.ctx, (R_Vertex) { r_p1, { u0, v0 }, g_core.color_1 });
				D_PushVertex(& g_core.ctx, (R_Vertex) { r_p2, { u1, v0 }, g_core.color_2 });
				D_PushVertex(& g_core.ctx, (R_Vertex) { r_p0, { u0, v1 }, g_core.color_0 });
				D_PushVertex(& g_core.ctx, (R_Vertex) { r_p2, { u1, v0 }, g_core.color_2 });
				D_PushVertex(& g_core.ctx, (R_Vertex) { r_p3, { u1, v1 }, g_core.color_3 });
			}
		}
	}
	ResetPerDrawState();
	return 0;
}


ELF_FUNCTION(L_NewFont) {
	int num_glyphs = elf_loadint(S, 1);

	// FONT_HANDLE font = NewFont(num_glyphs);
	// elf_pushsys(S, (elf_Handle) font);
	elf_pushnil(S);
	return 1;
}

ELF_FUNCTION(L_SetFontGlyph) {
	int nextarg = 1;
	// ADD_FONT_GLYPH params = {};
	// params.rune      = elf_loadint(S, nextarg ++);
	// params.src       =  load_image(S, nextarg ++);
	// params.src_x     = elf_loadint(S, nextarg ++);
	// params.src_y     = elf_loadint(S, nextarg ++);
	// params.src_w     = elf_loadint(S, nextarg ++);
	// params.src_h     = elf_loadint(S, nextarg ++);
	// params.x_bearing = elf_loadint(S, nextarg ++);
	// params.y_bearing = elf_loadint(S, nextarg ++);
	// params.x_advance = elf_loadint(S, nextarg ++);
	// params.user_data = elf_loadint(S, nextarg ++);
	// SetFontGlyph(g_core.font, params);
	return 0;
}


elf_Binding l_font[] = {
	{ "NewFont"              , L_NewFont           },
	{ "SetFontGlyph"         , L_SetFontGlyph      },
	{ "DrawText"             , L_DrawText          },
	{ "MeasureText"          , L_MeasureText       },
	{ "LoadFont"             , L_LoadFont          },
	{ "SetFont"              , L_SetFont           },
};
