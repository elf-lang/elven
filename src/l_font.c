

ELF_FUNCTION(L_SetFont)
{
	FONT_HANDLE font = (FONT_HANDLE) elf_loadsys(S, 1);
	gd.font = font;
	return 0;
}



ELF_FUNCTION(L_LoadFont) {

	const char *name = elf_loadtext(S, 1);
	int size = elf_loadint(S, 2);

	D_FONT *font = InstallFont((char *) name, size);
	if (font) {
		elf_pushsys(S, (elf_Handle) font);
	}
	else {
		elf_pushnil(S);
	}
	return 1;
}



ELF_FUNCTION(L_MeasureText) {
	const char *text = elf_loadtext(S, 1);

	f32 w = MeasureText(text);
	elf_pushnum(S, w);
	return 1;
}



ELF_FUNCTION(L_DrawText) {

	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	const char *text = elf_loadtext(S, 3);

	D_DrawText(x, y, text);
	return 0;
}



elf_Binding l_font[] = {
	{ "DrawText"             , L_DrawText          },
	{ "MeasureText"          , L_MeasureText       },
	{ "LoadFont"             , L_LoadFont          },
	{ "SetFont"              , L_SetFont           },
};
