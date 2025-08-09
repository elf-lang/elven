struct {
	R_Renderer *rend;
	OS_WindowId window;
	i64 begin_cycle_clock;
	f64 clocks_to_seconds;
	f64 target_seconds_to_sleep;
	f64 pending_seconds_to_sleep;
	vec2 mouse;
	b32 targetsurface;
} gl;


// todo: if it's just one value then grayscale
// todo: color modes for HSV!
static inline Color _get_color_arg(elf_State *S, int args, int nargs) {
	Color color = {0,0,0,255};
	if (nargs >= 4) {
		color.r = elf_toint(S, args + 1);
		color.g = elf_toint(S, args + 2);
		color.b = elf_toint(S, args + 3);
		if (nargs >= 5) {
			color.a = elf_toint(S, args + 4);
		}
	}
	return color;
}

ELF_FUNCTION(L_SetAlpha) {
	D_SetAlpha(elf_toint(S, args + 1));
	return 0;
}

ELF_FUNCTION(L_SetColor0) {
	D_SetColor0(_get_color_arg(S, args, nargs));
	return 0;
}

ELF_FUNCTION(L_SetColor1) {
	D_SetColor1(_get_color_arg(S, args, nargs));
	return 0;
}

ELF_FUNCTION(L_SetColor2) {
	D_SetColor2(_get_color_arg(S, args, nargs));
	return 0;
}

ELF_FUNCTION(L_SetColor3) {
	D_SetColor3(_get_color_arg(S, args, nargs));
	return 0;
}


ELF_FUNCTION(L_SetColor) {
	D_SetColor(_get_color_arg(S, args, nargs));
	return 0;
}


ELF_FUNCTION(L_SetColorP) {
	elf_u32 packed = elf_toint(S, args + 1);
	D_SetColor((Color){ packed >> 24, packed >> 16, packed >> 8, packed >> 0 });
	return 0;
}


ELF_FUNCTION(L_SolidFill) {
	R_Renderer *rend = gl.rend;
	D_SolidFill(rend);
	return 0;
}

ELF_FUNCTION(L_SetTexture) {

	R_Renderer *rend = gl.rend;

	TextureId id = elf_toint(S, args + 1 + 0);
	D_SetTexture(rend, id);
	return 0;
}

ELF_FUNCTION(L_SetRegion) {

	R_Renderer *rend = gl.rend;

	i32 x0 = elf_tonum(S, args + 1);
	i32 y0 = elf_tonum(S, args + 2);
	i32 x1 = x0 + elf_tonum(S, args + 3);
	i32 y1 = y0 + elf_tonum(S, args + 4);
	D_SetRegion(x0, y0, x1, y1);
	return 0;
}

ELF_FUNCTION(L_SetRotation) {
	D_SetRotation(elf_tonum(S, args + 1));
	return 0;
}

ELF_FUNCTION(L_SetCenter) {
	f32 x = elf_tonum(S, args + 1);
	f32 y = elf_tonum(S, args + 2);
	D_SetCenter(x, y);
	return 0;
}

ELF_FUNCTION(L_SetScale) {
	f32 x = elf_tonum(S, args + 1);
	f32 y = elf_tonum(S, args + 2);
	D_SetScale(x, y);
	return 0;
}


ELF_FUNCTION(L_Translate) {
	f32 x = elf_tonum(S, args + 1);
	f32 y = elf_tonum(S, args + 2);
	D_Translate(x, y);
	return 0;
}

ELF_FUNCTION(L_GetTranslation) {
	return 0;
}


// todo: valid?
ELF_FUNCTION(L_SetOffset) {
	f32 x = elf_tonum(S, args + 1);
	f32 y = elf_tonum(S, args + 2);
	D_SetOffset(x, y);
	return 0;
}

ELF_FUNCTION(L_SetFlipOnce) {
	int x = elf_toint(S, args + 1 + 0);
	int y = elf_toint(S, args + 1 + 1);
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

	R_Renderer *rend = gl.rend;

	Color color = _get_color_arg(S, args, nargs);
	D_Clear(rend, color);
	return 0;
}

ELF_FUNCTION(L_DrawTriangle) {

	R_Renderer *rend = gl.rend;

	f32 x0 = elf_tonum(S, args + 1);
	f32 y0 = elf_tonum(S, args + 2);

	f32 x1 = elf_tonum(S, args + 3);
	f32 y1 = elf_tonum(S, args + 4);

	f32 x2 = elf_tonum(S, args + 5);
	f32 y2 = elf_tonum(S, args + 6);

	D_DrawTriangle(rend, x0, y0, x1, y1, x2, y2);
	return 0;
}



ELF_FUNCTION(L_DrawLine) {

	R_Renderer *rend = gl.rend;

	f32 x0 = elf_tonum(S, args + 1);
	f32 y0 = elf_tonum(S, args + 2);
	f32 x1 = elf_tonum(S, args + 3);
	f32 y1 = elf_tonum(S, args + 4);

	D_DrawLine(rend, x0, y0, x1, y1);
	return 0;
}

ELF_FUNCTION(L_DrawCircle) {

	R_Renderer *rend = gl.rend;

	f32 x = elf_tonum(S, args + 1);
	f32 y = elf_tonum(S, args + 2);
	f32 r = elf_tonum(S, args + 3);
	f32 v = elf_tonum(S, args + 4);

	D_DrawCircle(rend, x, y, r, v);
	return 0;
}

ELF_FUNCTION(L_DrawRectangle) {

	R_Renderer *rend = gl.rend;

	f32 x = elf_tonum(S, args + 1);
	f32 y = elf_tonum(S, args + 2);
	f32 w = elf_tonum(S, args + 3);
	f32 h = elf_tonum(S, args + 4);

	D_DrawRectangle(rend, x, y, w, h);
	return 0;
}

ELF_FUNCTION(L_GetTextureResolution) {

	R_Renderer *rend = gl.rend;

	int id = elf_toint(S, args + 1 + 0);
	vec2i res = R_GetTextureInfo(rend, id);

	elf_pushtab(S);

	elf_pushstr(S, "x");
	elf_pushint(S, res.x);
	elf_setfield(S);

	elf_pushstr(S, "y");
	elf_pushint(S, res.y);
	elf_setfield(S);
	return 1;
}

ELF_FUNCTION(L_LoadTexture) {

	R_Renderer *rend = gl.rend;

	char *name = f_checktext(S, 0);

	i32 c;

	vec2i resolution = {};
	Color *colors = (Color *) stbi_load(name, &resolution.x, &resolution.y, &c, 4);

	// we create a new id automatically, because the user doesn't
	// know which ids are taken already, unless we added a custom
	// offset here #todo
	TextureId id = R_FindFreeTexture(rend);
	R_InstallTexture(rend, id, FORMAT_R8G8B8_UNORM, resolution, colors);

	free(colors);

	elf_pushint(S, id);
	return 1;
}

// todo: possibly rename to "Keyboard" and then MouseButton to "Mouse"
ELF_FUNCTION(L_Button) {
	OS_WindowId window = gl.window;
	int state = 0;
	for (int i = 1; i < nargs; i ++) {
		i32 index = elf_toint(S, args + i);
		state |= OS_GetWindowKey(window, index);
	}
	elf_pushint(S, state);
	return 1;
}

ELF_FUNCTION(L_MouseButton) {

	OS_WindowId window = gl.window;

	i32 index = elf_toint(S, args + 1 + 0);
	elf_pushint(S, OS_GetWindowKey(window, KEY_MOUSE_LEFT + index));
	return 1;
}

ELF_FUNCTION(L_GetMouseWheel) {

	OS_WindowId window = gl.window;

	elf_pushint(S, OS_GetWindowMouseWheel(window).y);
	return 1;
}

ELF_FUNCTION(L_GetMouseX) {

	elf_pushnum(S, gl.mouse.x);
	return 1;
}

ELF_FUNCTION(L_GetMouseY) {

	elf_pushnum(S, gl.mouse.y);
	return 1;
}



static void BeginDrawing(R_Renderer *rend) {
	R_BeginFrame(rend);

	D_SetScale(1, 1);
	D_SetOffset(0, 0);
	D_SetCenter(0, 0);
	// you don't want to do this
	// D_SetFont(0);
	D_SetTexture(rend, TEXTURE_DEFAULT);

	R_SetSurface(rend, gl.targetsurface);
	R_SetSampler(rend, SAMPLER_POINT);
	R_SetViewportFullScreen(rend);
	R_SetTopology(rend, TOPO_TRIANGLES);
	R_SetBlender(rend, BLENDER_ALPHA_BLEND);

	D_LoadIdentity();
}

ELF_FUNCTION(L_InitWindow) {

	const char *name = elf_tostr(S, args + 1);

	vec2i resolution = { 0 };
	vec2i windowreso = { 0 };


	if (nargs >= 4) {
		resolution.x = windowreso.x = elf_toint(S, args + 2);
		resolution.y = windowreso.y = elf_toint(S, args + 3);
		if (nargs >= 5) {
			int windowscale = elf_toint(S, args + 4);
			windowreso.x *= windowscale;
			windowreso.y *= windowscale;
		}
	}

	TRACELOG("Init Window... %ix%i", windowreso.x, windowreso.y);

	OS_InstallWindow(WINDOW_MAIN, name, windowreso);

	TRACELOG("Init Renderer...");
	R_Renderer *rend = R_InitRenderer(WINDOW_MAIN);
	gl.rend = rend;
	gl.window = WINDOW_MAIN;
	gl.targetsurface = TEXTURE_RT_WINDOW;

	if (resolution.x > 0 && resolution.y > 0) {
		gl.targetsurface = TEXTURE_RT_BASE;
		R_InstallSurface(rend, TEXTURE_RT_BASE, FORMAT_R8G8B8_UNORM, resolution);
	}

	gl.target_seconds_to_sleep = 1.0 / 60.0;
	gl.begin_cycle_clock = OS_GetTickCounter();

	// OS_PollWindow(window);
	BeginDrawing(rend);
	return 0;
}

ELF_FUNCTION(L_PollWindow) {

	R_Renderer *rend = gl.rend;
	OS_WindowId window = gl.window;

	b32 open = OS_PollWindow(window);

	{
		if (gl.targetsurface != TEXTURE_RT_WINDOW) {
			R_Blit(rend, TEXTURE_RT_WINDOW, gl.targetsurface);
		}
		R_EndFrame(rend);
	}

	if (!open) goto esc;

	// because we only have one window
	OS_ForgetFileDrops();

	// begin a new frame
	BeginDrawing(rend);

	iRect rect = R_GetBlitRect(rend, TEXTURE_RT_WINDOW, gl.targetsurface);
	vec2i mouse = OS_GetWindowMouse(window);

	// can be negative
	gl.mouse.x = 0 + (mouse.x - rect.x) / (f32) rect.w;
	gl.mouse.y = 1 - (mouse.y - rect.y) / (f32) rect.h;

	// todo: fix this thing, it works too well
	i64 end_cycle_clock = OS_GetTickCounter();
	i64 elapsed_clocks = end_cycle_clock - gl.begin_cycle_clock;
	f64 elapsed_seconds = elapsed_clocks * OS_GetClocksToSeconds();
	gl.pending_seconds_to_sleep += gl.target_seconds_to_sleep - elapsed_seconds;

	// todo: instead of making this logic work in seconds
	// make it work in clock cycles directly
	f64 clock_accuracy = 1.0 / 1000.0;
	gl.begin_cycle_clock = OS_GetTickCounter();
	while (gl.pending_seconds_to_sleep > clock_accuracy) {
		OS_Sleep(gl.pending_seconds_to_sleep * 1000);
		gl.begin_cycle_clock = OS_GetTickCounter();
		gl.pending_seconds_to_sleep -= (gl.begin_cycle_clock - end_cycle_clock) * OS_GetClocksToSeconds();
	}

	esc:
	elf_pushint(S, open);
	return 1;
}

ELF_FUNCTION(L_SetFont) {
	D_SetFont(elf_toint(S, args + 1 + 0));
	return 0;
}

ELF_FUNCTION(L_LoadFont) {

	R_Renderer *rend = gl.rend;

	int slot = elf_toint(S, args + 1 + 0);
	char *name = f_checktext(S, 1);
	int size = elf_toint(S, args + 1 + 2);

	int id = InstallFont(rend, slot, name, size);
	elf_pushint(S, id);
	return 1;
}

ELF_FUNCTION(L_MeasureText) {
	const char *text = elf_tostr(S, args + 1);

	f32 w = D_MeasureText(text);
	elf_pushnum(S, w);
	return 1;
}

ELF_FUNCTION(L_DrawText) {
	R_Renderer *rend = gl.rend;

	f32 x = elf_tonum(S, args + 1);
	f32 y = elf_tonum(S, args + 2);
	const char *text = elf_tostr(S, args + 3);

	D_DrawText(rend, x, y, text);
	return 0;
}

ELF_FUNCTION(L_GetFileDrop) {

	OS_WindowId window = gl.window;
	i32 index = elf_toint(S, args + 1 + 0);
	elf_pushstr(S, OS_GetFileDrop(index));
	return 1;
}

ELF_FUNCTION(L_GetNumFileDrops) {

	OS_WindowId window = gl.window;
	elf_pushint(S, OS_GetNumFileDrops());
	return 1;
}

ELF_FUNCTION(L_OpenFileDialog) {

	char *path = f_checktext(S, 0);

	char buffer[256] = {};
	int result = OS_OpenFileDialog(path, buffer, sizeof(buffer));
	if (result) {
		elf_pushstr(S, buffer);
	} else {
		elf_pushnil(S);
	}
	return 1;
}

// # AUDIO
ELF_FUNCTION(L_GetNumVoices) {
	int numvoices = A_GetNumVoices();
	elf_pushint(S, numvoices);
	return 1;
}

ELF_FUNCTION(L_GetVoiceSound) {
	int voiceid = elf_toint(S, args + 1 + 0);
	int sound = A_GetVoiceSound(voiceid);
	elf_pushint(S, sound);
	return 1;
}

ELF_FUNCTION(L_InitAudio) {
	int error = A_InitAudio();
	elf_pushint(S, error);
	return 1;
}

ELF_FUNCTION(L_LoadSound) {
	int id = elf_toint(S, args + 1 + 0);
	char *name = f_checktext(S, 1);
	int error = A_LoadSoundFromFile(id, name);
	elf_pushint(S, error);
	return 1;
}

ELF_FUNCTION(L_PlaySound) {
	int id = elf_toint(S, args + 1 + 0);
	int voiceid = A_PlaySound(id);
	elf_pushint(S, voiceid);
	return 1;
}

ELF_FUNCTION(L_StopVoice) {
	int id = elf_toint(S, args + 1 + 0);
	A_StopVoice(id);
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
	{ "SetAlpha"                         , L_SetAlpha                             },
	{ "SetColorP"                        , L_SetColorP                            },
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
	{ "SetFlipOnce"                      , L_SetFlipOnce                          },

	{ "DrawText"                         , L_DrawText                             },
	{ "MeasureText"                      , L_MeasureText                             },


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
