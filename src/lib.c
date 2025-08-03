struct {
	R_Renderer *rend;
	OS_WindowId window;
	i64 begin_cycle_clock;
	f64 clocks_to_seconds;
	f64 target_seconds_to_sleep;
	f64 pending_seconds_to_sleep;
	vec2 mouse;
} glib;


// todo: if it's just one value then grayscale
// todo: color modes for HSV!
static inline Color _get_color_arg(elf_State *S) {
	Color color = {};
	if (elf_get_num_args(S) > 2) {
		color.r = elf_get_intarg(S, 0);
		color.g = elf_get_intarg(S, 1);
		color.b = elf_get_intarg(S, 2);
		if (elf_get_num_args(S) > 3) {
			color.a = elf_get_intarg(S, 3);
		}
	}
	return color;
}

ELF_FUNCTION(L_SetAlpha) {
	D_SetAlpha(elf_get_intarg(S, 0));
	return 0;
}

ELF_FUNCTION(L_SetColor0) {
	D_SetColor0(_get_color_arg(S));
	return 0;
}

ELF_FUNCTION(L_SetColor1) {
	D_SetColor1(_get_color_arg(S));
	return 0;
}

ELF_FUNCTION(L_SetColor2) {
	D_SetColor2(_get_color_arg(S));
	return 0;
}

ELF_FUNCTION(L_SetColor3) {
	D_SetColor3(_get_color_arg(S));
	return 0;
}

ELF_FUNCTION(L_SetColor) {
	D_SetColor(_get_color_arg(S));
	return 0;
}

ELF_FUNCTION(L_SolidFill) {
	R_Renderer *rend = glib.rend;
	D_SolidFill(rend);
	return 0;
}

ELF_FUNCTION(L_SetTexture) {

	R_Renderer *rend = glib.rend;

	TextureId id = elf_get_intarg(S, 0);
	D_SetTexture(rend, id);
	return 0;
}

ELF_FUNCTION(L_SetRegion) {

	R_Renderer *rend = glib.rend;

	i32 x0 = elf_get_numarg(S, 0);
	i32 y0 = elf_get_numarg(S, 1);
	i32 x1 = x0 + elf_get_numarg(S, 2);
	i32 y1 = y0 + elf_get_numarg(S, 3);
	D_SetRegion(x0, y0, x1, y1);
	return 0;
}

ELF_FUNCTION(L_SetRotation) {
	D_SetRotation(elf_get_numarg(S,0));
	return 0;
}

ELF_FUNCTION(L_SetCenter) {
	f32 x = elf_get_numarg(S,0);
	f32 y = elf_get_numarg(S,1);
	D_SetCenter(x, y);
	return 0;
}

ELF_FUNCTION(L_SetScale) {
	f32 x = elf_get_numarg(S,0);
	f32 y = elf_get_numarg(S,1);
	D_SetScale(x, y);
	return 0;
}


ELF_FUNCTION(L_Translate) {
	f32 x = elf_get_numarg(S,0);
	f32 y = elf_get_numarg(S,1);
	D_Translate(x, y);
	return 0;
}

ELF_FUNCTION(L_GetTranslation) {
	elf_error(S, -1, "Not Impl!");
	return 0;
}


// todo: valid?
ELF_FUNCTION(L_SetOffset) {
	f32 x = elf_get_numarg(S,0);
	f32 y = elf_get_numarg(S,1);
	D_SetOffset(x, y);
	return 0;
}

ELF_FUNCTION(L_SetFlipOnce) {
	int x = elf_get_intarg(S,0);
	int y = elf_get_intarg(S,1);
	D_SetFlipOnce(x, y);
	return 0;
}


ELF_FUNCTION(L_PushMatrix) {
	D_PushMatrix();
	return 0;
}

ELF_FUNCTION(L_PopMatrix) {
	D_PopMatrix();
	return 0;
}



ELF_FUNCTION(L_Clear) {

	R_Renderer *rend = glib.rend;

	Color color = _get_color_arg(S);
	D_Clear(rend, color);
	return 0;
}

ELF_FUNCTION(L_DrawTriangle) {

	R_Renderer *rend = glib.rend;

	f32 x0 = elf_get_numarg(S, 0);
	f32 y0 = elf_get_numarg(S, 1);

	f32 x1 = elf_get_numarg(S, 2);
	f32 y1 = elf_get_numarg(S, 3);

	f32 x2 = elf_get_numarg(S, 4);
	f32 y2 = elf_get_numarg(S, 5);

	D_DrawTriangle(rend, x0, y0, x1, y1, x2, y2);
	return 0;
}



ELF_FUNCTION(L_DrawLine) {

	R_Renderer *rend = glib.rend;

	f32 x0 = elf_get_numarg(S, 0);
	f32 y0 = elf_get_numarg(S, 1);
	f32 x1 = elf_get_numarg(S, 2);
	f32 y1 = elf_get_numarg(S, 3);

	D_DrawLine(rend, x0, y0, x1, y1);
	return 0;
}

ELF_FUNCTION(L_DrawCircle) {

	R_Renderer *rend = glib.rend;

	f32 x = elf_get_numarg(S, 0);
	f32 y = elf_get_numarg(S, 1);
	f32 r = elf_get_numarg(S, 2);
	f32 v = elf_get_numarg(S, 3);

	D_DrawCircle(rend, x, y, r, v);
	return 0;
}

ELF_FUNCTION(L_DrawRectangle) {

	R_Renderer *rend = glib.rend;

	f32 x = elf_get_numarg(S, 0);
	f32 y = elf_get_numarg(S, 1);
	f32 w = elf_get_numarg(S, 2);
	f32 h = elf_get_numarg(S, 3);

	D_DrawRectangle(rend, x, y, w, h);
	return 0;
}

ELF_FUNCTION(L_GetTextureResolution) {

	R_Renderer *rend = glib.rend;

	int id = elf_get_intarg(S, 0);
	vec2i res = R_GetTextureInfo(rend, id);

	elf_push_table(S);

	elf_push_string(S, "x");
	elf_push_int(S, res.x);
	elf_table_set(S);

	elf_push_string(S, "y");
	elf_push_int(S, res.y);
	elf_table_set(S);
	return 1;
}

ELF_FUNCTION(L_LoadTexture) {

	R_Renderer *rend = glib.rend;

	char *name = elf_get_text_arg(S, 0);

	i32 c;

	vec2i resolution = {};
	Color *colors = (Color *) stbi_load(name, &resolution.x, &resolution.y, &c, 4);

	// we create a new id automatically, because the user doesn't
	// know which ids are taken already, unless we added a custom
	// offset here #todo
	TextureId id = R_FindFreeTexture(rend);
	R_InstallTexture(rend, id, FORMAT_R8G8B8_UNORM, resolution, colors);

	free(colors);

	elf_push_int(S, id);
	return 1;
}

ELF_FUNCTION(L_Button) {

	OS_WindowId window = glib.window;

	i32 index = elf_get_intarg(S,0);
	elf_push_int(S, OS_GetWindowKey(window, index));
	return 1;
}

ELF_FUNCTION(L_MouseButton) {

	OS_WindowId window = glib.window;

	i32 index = elf_get_intarg(S,0);
	elf_push_int(S, OS_GetWindowKey(window, KEY_MOUSE_LEFT + index));
	return 1;
}

ELF_FUNCTION(L_GetMouseWheel) {

	OS_WindowId window = glib.window;

	elf_push_int(S, OS_GetWindowMouseWheel(window).y);
	return 1;
}

ELF_FUNCTION(L_GetMouseX) {

	elf_push_num(S, glib.mouse.x);
	return 1;
}

ELF_FUNCTION(L_GetMouseY) {

	elf_push_num(S, glib.mouse.y);
	return 1;
}


ELF_FUNCTION(L_InitWindow) {


	int i = 0;
	char *name = elf_get_text_arg(S,i++);
	int resolution_x = elf_get_intarg(S,i++);
	int resolution_y = elf_get_intarg(S,i++);
	int window_scale = elf_get_intarg(S,i++);

	if (window_scale <= 0) {
		window_scale = 1;
	}

	OS_InstallWindow(WINDOW_MAIN, name, (vec2i){ resolution_x * window_scale, resolution_y * window_scale });
	R_Renderer *rend = R_InitRenderer(WINDOW_MAIN);
	glib.rend = rend;
	glib.window = WINDOW_MAIN;

	R_InstallSurface(rend, TEXTURE_RT_BASE, FORMAT_R8G8B8_UNORM, (vec2i){ resolution_x, resolution_y });

	glib.target_seconds_to_sleep = 1.0 / 60.0;
	glib.begin_cycle_clock = OS_GetClock();

	// OS_PollWindow(window);
	D_BeginDrawing(rend);
	return 0;
}

ELF_FUNCTION(L_PollWindow) {

	R_Renderer *rend = glib.rend;
	OS_WindowId window = glib.window;

	// because we only have one window
	OS_ForgetFileDrops();
	b32 open = OS_PollWindow(window);

	iRect rect = R_GetBlitRect(rend, TEXTURE_RT_WINDOW, TEXTURE_RT_BASE);
	vec2i mouse = OS_GetWindowMouse(window);
	glib.mouse.x = 0 + (mouse.x - rect.x) / (f32) rect.w;
	glib.mouse.y = 1 - (mouse.y - rect.y) / (f32) rect.h;
	// let it be negative because this way the user knows it is out of bounds
	// glib.mouse.x = glib.mouse.x < 0 ? 0 : glib.mouse.x > 1.0 ? 1.0 : glib.mouse.x;
	// glib.mouse.y = glib.mouse.y < 0 ? 0 : glib.mouse.y > 1.0 ? 1.0 : glib.mouse.y;

	D_EndDrawing(rend);
	D_BeginDrawing(rend);

	// todo: fix this thing, it works too well
	i64 end_cycle_clock = OS_GetClock();
	i64 elapsed_clocks = end_cycle_clock - glib.begin_cycle_clock;
	f64 elapsed_seconds = elapsed_clocks * OS_GetClocksToSeconds();
	glib.pending_seconds_to_sleep += glib.target_seconds_to_sleep - elapsed_seconds;

	// todo: instead of making this logic work in seconds
	// make it work in clock cycles directly
	f64 clock_accuracy = 1.0 / 1000.0;
	glib.begin_cycle_clock = OS_GetClock();
	while (glib.pending_seconds_to_sleep > clock_accuracy) {
		OS_Sleep(glib.pending_seconds_to_sleep * 1000);
		glib.begin_cycle_clock = OS_GetClock();
		glib.pending_seconds_to_sleep -= (glib.begin_cycle_clock - end_cycle_clock) * OS_GetClocksToSeconds();
	}

	elf_push_int(S, open);
	return 1;
}

ELF_FUNCTION(L_SetFont) {
	D_SetFont(elf_get_intarg(S, 0));
	return 0;
}

ELF_FUNCTION(L_LoadFont) {

	R_Renderer *rend = glib.rend;

	int slot = elf_get_intarg(S, 0);
	char *name = elf_get_text_arg(S, 1);
	int size = elf_get_intarg(S, 2);

	int id = InstallFont(rend, slot, name, size);
	elf_push_int(S, id);
	return 1;
}

ELF_FUNCTION(L_DrawText) {

	R_Renderer *rend = glib.rend;

	f32 x = elf_get_numarg(S, 0);
	f32 y = elf_get_numarg(S, 1);
	char *text = elf_get_text_arg(S, 2);

	D_DrawText(rend, x, y, text);
	return 0;
}

ELF_FUNCTION(L_GetFileDrop) {

	OS_WindowId window = glib.window;
	i32 index = elf_get_intarg(S, 0);
	elf_push_string(S, OS_GetFileDrop(index));
	return 1;
}

ELF_FUNCTION(L_GetNumFileDrops) {

	OS_WindowId window = glib.window;
	elf_push_int(S, OS_GetNumFileDrops());
	return 1;
}

ELF_FUNCTION(L_OpenFileDialog) {

	char *path = elf_get_text_arg(S, 0);

	char buffer[256] = {};
	int result = OS_OpenFileDialog(path, buffer, sizeof(buffer));
	if (result) {
		elf_push_string(S, buffer);
	} else {
		elf_push_nil(S);
	}
	return 1;
}

// # AUDIO
ELF_FUNCTION(L_GetNumVoices) {
	int numvoices = GetNumVoices();
	elf_push_int(S, numvoices);
	return 1;
}

ELF_FUNCTION(L_GetVoiceSound) {
	int voiceid = elf_get_intarg(S, 0);
	int sound = GetVoiceSound(voiceid);
	elf_push_int(S, sound);
	return 1;
}

ELF_FUNCTION(L_InitAudio) {
	int error = A_InitAudio();
	elf_push_int(S, error);
	return 1;
}

ELF_FUNCTION(L_LoadSound) {
	int id = elf_get_intarg(S, 0);
	char *name = elf_get_text_arg(S, 1);
	ASSERT(id < SOUNDS_CAPACITY);

	MSOUND *msnd = &g_sounds[id];
	ma_sound *sound = (ma_sound *) msnd;
	if (msnd->loaded) {
		ma_sound_uninit(sound);
	}

	// todo: remove this please!
	wchar_t namew[1024] = {};
	MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, name, -1, namew, sizeof(namew));

	ma_result error = ma_sound_init_from_file_w(&g_engine, namew, 0, NULL, NULL, sound);
	if (error == MA_SUCCESS) {
		msnd->loaded = true;
	}
	elf_push_int(S, error == MA_SUCCESS);
	return 1;
}

ELF_FUNCTION(L_PlaySound) {
	int id = elf_get_intarg(S, 0);
	int voiceid = PlaySound(id);
	elf_push_int(S, voiceid);
	return 1;
}

ELF_FUNCTION(L_StopVoice) {
	int id = elf_get_intarg(S, 0);
	StopVoice(id);
	return 0;
}


static const elf_Binding g_lib[] = {
	{ "InitWindow"                       , L_InitWindow                           },
	{ "PollWindow"                       , L_PollWindow                           },
	{ "LoadTexture"                      , L_LoadTexture                          },
	{ "GetTextureResolution"             , L_GetTextureResolution                 },
	{ "Button"                           , L_Button                               },
	{ "MouseButton"                      , L_MouseButton                          },
	{ "Clear"                            , L_Clear                                },
	{ "GetMouseWheel"                    , L_GetMouseWheel                        },
	{ "GetMouseX"                        , L_GetMouseX                            },
	{ "GetMouseY"                        , L_GetMouseY                            },
	{ "Translate"                        , L_Translate                            },
	{ "GetTranslation"                   , L_GetTranslation                       },
	{ "SetScale"                         , L_SetScale                             },
	{ "SetOffset"                        , L_SetOffset                            },
	{ "SetRotation"                      , L_SetRotation                          },
	{ "SetTexture"                       , L_SetTexture                           },
	{ "SetRegion"                        , L_SetRegion                            },
	{ "SetCenter"                        , L_SetCenter                            },
	{ "SetColor"                         , L_SetColor                             },
	{ "SetColor0"                        , L_SetColor0                            },
	{ "SetColor1"                        , L_SetColor1                            },
	{ "SetColor2"                        , L_SetColor2                            },
	{ "SetColor3"                        , L_SetColor3                            },
	{ "SolidFill"                        , L_SolidFill                            },
	{ "DrawRectangle"                    , L_DrawRectangle                        },
	{ "DrawTriangle"                     , L_DrawTriangle                         },
	{ "DrawLine"                         , L_DrawLine                             },
	{ "DrawCircle"                       , L_DrawCircle                           },
	{ "DrawText"                         , L_DrawText                             },
	{ "SetFlipOnce"                      , L_SetFlipOnce                          },


	{ "LoadFont"                         , L_LoadFont                             },
	{ "SetFont"                          , L_SetFont                              },

	{ "PushMatrix"                       , L_PushMatrix                           },
	{ "PopMatrix"                        , L_PopMatrix                            },

	{ "GetFileDrop"                      , L_GetFileDrop                          },
	{ "GetNumFileDrops"                  , L_GetNumFileDrops                      },
	{ "OpenFileDialog"                   , L_OpenFileDialog                       },

	{ "InitAudio"                        , L_InitAudio                            },
	{ "LoadSound"                        , L_LoadSound                            },
	{ "PlaySound"                        , L_PlaySound                            },
	{ "GetVoiceSound"                    , L_GetVoiceSound                        },
	{ "GetNumVoices"                     , L_GetNumVoices                         },
	{ "StopVoice"                        , L_StopVoice                            },
};
