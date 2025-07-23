
static int L_Button(elf_State *S);
static int L_GetCursorX(elf_State *S);
static int L_GetCursorY(elf_State *S);
static int L_InitWindow(elf_State *S);
static int L_PollWindow(elf_State *S);
int L_SetColor(elf_State *S);
int L_SetRotation(elf_State *S);
int L_SetCenter(elf_State *S);
int L_SetScale(elf_State *S);
int L_SetOffset(elf_State *S);
int L_Clear(elf_State *S);
int L_DrawTriangle(elf_State *S);
int L_DrawLine(elf_State *S);
int L_DrawCircle(elf_State *S);
int L_LoadFont(elf_State *S);
int L_SolidFill(elf_State *S);
int L_DrawText(elf_State *S);
int L_DrawRectangle(elf_State *S);



NameFunctionPair lib[] = {
	{ "InitWindow", L_InitWindow },
	{ "PollWindow", L_PollWindow },
	{ "LoadFont", L_LoadFont },
	{ "Button", L_Button },
	{ "Clear", L_Clear },
	{ "GetCursorX", L_GetCursorX },
	{ "GetCursorY", L_GetCursorY },
	{ "SetScale", L_SetScale },
	{ "SetOffset", L_SetOffset },
	{ "SetRotation", L_SetRotation },
	{ "SetCenter", L_SetCenter },
	{ "SetColor", L_SetColor },
	{ "DrawText", L_DrawText },
	{ "SolidFill", L_SolidFill },
	{ "DrawRectangle", L_DrawRectangle },
	{ "DrawTriangle", L_DrawTriangle },
	{ "DrawLine", L_DrawLine },
	{ "DrawCircle", L_DrawCircle },
		// {"elf.jam.audio", l_audio},
		// { "LoadTexture", L_LoadTexture },
		// {"elf.jam.audio_play", l_audio_play},
		// {"elf.jam.audio_load", l_audio_load},
		// {"elf.jam.audio_stop", l_audio_stop},
		// {"elf.jam.load_image", j_load_image},
		// {"elf.jam.get_image_pixel", j_get_image_pixel},
		// {"elf.jam.get_image_size", j_get_image_size},
		// {"elf.jam.get_texture_size", l_get_texture_size},
		// {"elf.jam.get_input", l_get_input},
};