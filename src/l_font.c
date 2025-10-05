
ELF_FUNCTION(L_SetFont)
{
	// todo:
	FONT_HANDLE font = (FONT_HANDLE) { (void *) elf_loadsys(S, 1) };
	gd.font = font;
	return 0;
}

ELF_FUNCTION(L_NewFont) {
	int num_glyphs = elf_loadint(S, 1);

	FONT_HANDLE font = NewFont(num_glyphs);
	elf_pushsys(S, (elf_Handle) font);
	return 1;
}

ELF_FUNCTION(L_SetFontGlyph) {
	int nextarg = 1;
	ADD_FONT_GLYPH params = {};
	params.rune      = elf_loadint(S, nextarg ++);
	params.src       =  load_image(S, nextarg ++);
	params.src_x     = elf_loadint(S, nextarg ++);
	params.src_y     = elf_loadint(S, nextarg ++);
	params.src_w     = elf_loadint(S, nextarg ++);
	params.src_h     = elf_loadint(S, nextarg ++);
	params.x_bearing = elf_loadint(S, nextarg ++);
	params.y_bearing = elf_loadint(S, nextarg ++);
	params.x_advance = elf_loadint(S, nextarg ++);
	params.user_data = elf_loadint(S, nextarg ++);
	SetFontGlyph(gd.font, params);
	return 0;
}

ELF_FUNCTION(L_LoadFont) {

	// todo: check file extension
	const char *name = elf_loadtext(S, 1);
	int font_size = elf_loadint(S, 2);

	FILE_HANDLE file = sys_open_file(name, SYS_OPEN_READ, SYS_OPEN_EXISTING);
	if (file != ELF_HINVALID) {
		int size = sys_size_file(file);

		void *data = malloc(size);
		sys_read_file(file, data, size);

		FONT_HANDLE font = LoadTrueTypeFont(data, font_size);
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

	f32 w = MeasureText(D_GetFont(), text);
	elf_pushnum(S, w);
	return 1;
}

static void DrawTextQuads(f32 x, f32 y, Text_Quad *quads, int nquads) {
	i32 num_verts = nquads * 6;
	R_Vertex3 *verts = BeginDrawCall(TOPO_TRIANGLES, num_verts);
	R_Vertex3 *cursor = verts;

	for (int i = 0; i < nquads; i ++) {
		Rect dst = quads[i].dst;

		vec3 r_p0 = { x + dst.x +     0, y + dst.y +     0, 0 };
		vec3 r_p1 = { x + dst.x +     0, y + dst.y + dst.h, 0 };
		vec3 r_p2 = { x + dst.x + dst.w, y + dst.y + dst.h, 0 };
		vec3 r_p3 = { x + dst.x + dst.w, y + dst.y +     0, 0 };

		f32 u0 = quads[i].src.u0;
		f32 v0 = quads[i].src.v0;
		f32 u1 = quads[i].src.u1;
		f32 v1 = quads[i].src.v1;

		cursor[0]=(R_Vertex3){r_p0,{u0,v1},gd.color_0};
		cursor[1]=(R_Vertex3){r_p1,{u0,v0},gd.color_1};
		cursor[2]=(R_Vertex3){r_p2,{u1,v0},gd.color_2};
		cursor[3]=(R_Vertex3){r_p0,{u0,v1},gd.color_0};
		cursor[4]=(R_Vertex3){r_p2,{u1,v0},gd.color_2};
		cursor[5]=(R_Vertex3){r_p3,{u1,v1},gd.color_3};
		cursor += 6;
	}

	EndDrawCall(verts, num_verts);
}

ELF_FUNCTION(L_DrawText) {

	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);

	int textl;
	const char *text = elf_loadtextl(S, 3, &textl);

	Draw_Text_Pass *pass = DrawText(D_GetFont(), text, textl);
	Text_Run *run;

	for (run = pass->runs; run < pass->runs + pass->nruns; ++ run) {
		D_SetTexture(run->texture);
		DrawTextQuads(x, y, run->quads, run->nquads);
	}
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
