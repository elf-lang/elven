



ELF_FUNCTION(L_SetFont)
{
	// todo:
	FONT_HANDLE font = (FONT_HANDLE) { (void *) elf_loadsys(S, 1) };
	gd.font = font;
	return 0;
}

//
//
//
//
//
//
//
//

ELF_FUNCTION(L_NewFont) {
	int num_glyphs = elf_loadint(S, 1);

	FONT_HANDLE font = NewFont(num_glyphs);
	elf_pushsys(S, (elf_Handle) font);

	return 1;
}

//
//
//
//
//
//
//
//

ELF_FUNCTION(L_SetFontTexture) {
	RID texture = (RID) elf_loadsys(S, 1);
	SetFontTexture(gd.font, texture);
	return 0;
}

//
//
//
//
//
//
//
//

ELF_FUNCTION(L_SetFontGlyph) {
	int nextarg = 1;
	int rune = elf_loadint(S, nextarg ++);
	Font_Glyph_Data data = {};
	data.x         = elf_loadint(S, nextarg ++);
	data.y         = elf_loadint(S, nextarg ++);
	data.w         = elf_loadint(S, nextarg ++);
	data.h         = elf_loadint(S, nextarg ++);
	data.x_bearing = elf_loadint(S, nextarg ++);
	data.y_bearing = elf_loadint(S, nextarg ++);
	data.x_advance = elf_loadint(S, nextarg ++);
	data.user_data = elf_loadint(S, nextarg ++);
	SetFontGlyph(gd.font, rune, data);
	return 0;
}

//
//
//
//
//
//
//
//

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

//
//
//
//

ELF_FUNCTION(L_MeasureText) {
	const char *text = elf_loadtext(S, 1);

	f32 w = MeasureText(D_GetFont(), text);
	elf_pushnum(S, w);
	return 1;
}

//
//
//
//
//
//
//
//

ELF_FUNCTION(L_DrawText) {

	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	const char *text = elf_loadtext(S, 3);

	DrawText(D_GetFont(), x, y, text);
	return 0;
}

//
//
//
//
//
//
//
//

elf_Binding l_font[] = {
	{ "NewFont"              , L_NewFont           },
	{ "SetFontGlyph"         , L_SetFontGlyph      },
	{ "SetFontTexture"       , L_SetFontTexture    },
	{ "DrawText"             , L_DrawText          },
	{ "MeasureText"          , L_MeasureText       },
	{ "LoadFont"             , L_LoadFont          },
	{ "SetFont"              , L_SetFont           },
};

