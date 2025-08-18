

D_DrawState gd;



// todo: if it's just one value then grayscale
// todo: color modes for HSV!
static inline Color _get_color_args2(elf_State *S, int index, int nargs) {
	nargs -= index;

	Color color = {0,0,0,255};
	if (nargs >= 3) {
		color.r = (u8) elf_loadint(S, index + 0);
		color.g = (u8) elf_loadint(S, index + 1);
		color.b = (u8) elf_loadint(S, index + 2);
		if (nargs >= 4) {
			color.a = (u8) elf_loadint(S, index + 3);
		}
	}
	return color;
}


static inline Color _get_color_arg(elf_State *S, int nargs) {
	return _get_color_args2(S, 1, nargs);
}



ELF_FUNCTION(L_SetAlpha) { D_SetAlpha((u8) elf_loadint(S, 1)); return 0; }

ELF_FUNCTION(L_SetColor0) { D_SetColor0(_get_color_arg(S, nargs)); return 0; }
ELF_FUNCTION(L_SetColor1) { D_SetColor1(_get_color_arg(S, nargs)); return 0; }
ELF_FUNCTION(L_SetColor2) { D_SetColor2(_get_color_arg(S, nargs)); return 0; }
ELF_FUNCTION(L_SetColor3) { D_SetColor3(_get_color_arg(S, nargs)); return 0; }

ELF_FUNCTION(L_SetColor)  { D_SetColor(_get_color_arg(S, nargs)); return 0; }



ELF_FUNCTION(L_SetViewport) {
	vec2i reso;
	reso.x = elf_loadint(S, 1);
	reso.y = elf_loadint(S, 2);
	R_SetViewport(gd.rend, reso);
	return 0;
}


ELF_FUNCTION(L_SetLayerColor)  {
	D_SetLayerColor(elf_loadint(S, 1), _get_color_args2(S, 2, nargs));
	return 0;
}

ELF_FUNCTION(L_SetLayerAlpha)  {
	D_SetLayerAlpha(elf_loadint(S, 1), elf_loadint(S, 2));
	return 0;
}


ELF_FUNCTION(L_SetColorP) {
	elf_u32 packed = elf_loadint(S, 1);
	D_SetColor((Color){ packed >> 24, packed >> 16, packed >> 8, packed >> 0 });
	return 0;
}


ELF_FUNCTION(L_SolidFill) {
	D_SolidFill();
	return 0;
}


ELF_FUNCTION(L_SetTexture) {
	RID id = (RID) elf_loadsys(S, 1);
	D_SetTexture(id);
	return 0;
}



ELF_FUNCTION(L_SetRegion) {
	i32 x0 = elf_loadnum(S, 1);
	i32 y0 = elf_loadnum(S, 2);
	i32 x1 = x0 + elf_loadnum(S, 3);
	i32 y1 = y0 + elf_loadnum(S, 4);
	D_SetRegion(x0, y0, x1, y1);
	return 0;
}




ELF_FUNCTION(L_SetRotation) {
	D_SetRotation(elf_loadnum(S, 1));
	return 0;
}




ELF_FUNCTION(L_SetCenter) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_SetCenter(x, y);
	return 0;
}



ELF_FUNCTION(L_SetScale) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_SetScale(x, y);
	return 0;
}



ELF_FUNCTION(L_Translate) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_Translate(x, y);
	return 0;
}



ELF_FUNCTION(L_GetTranslation) {
	return 0;
}



ELF_FUNCTION(L_SetOffset) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	D_SetOffset(x, y);
	return 0;
}



ELF_FUNCTION(L_SetFlipOnce) {
	int x = elf_loadint(S, 1);
	int y = elf_loadint(S, 2);
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
	Color color = _get_color_arg(S, nargs);
	D_Clear(color);
	return 0;
}



ELF_FUNCTION(L_DrawTriangle) {
	f32 x0 = elf_loadnum(S, 1);
	f32 y0 = elf_loadnum(S, 2);

	f32 x1 = elf_loadnum(S, 3);
	f32 y1 = elf_loadnum(S, 4);

	f32 x2 = elf_loadnum(S, 5);
	f32 y2 = elf_loadnum(S, 6);

	D_DrawTriangle(x0, y0, x1, y1, x2, y2);
	return 0;
}



ELF_FUNCTION(L_DrawLine) {
	f32 x0 = elf_loadnum(S, 1);
	f32 y0 = elf_loadnum(S, 2);
	f32 x1 = elf_loadnum(S, 3);
	f32 y1 = elf_loadnum(S, 4);

	D_DrawLine(x0, y0, x1, y1);
	return 0;
}



ELF_FUNCTION(L_DrawCircle) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	f32 r = elf_loadnum(S, 3);
	f32 v = elf_loadnum(S, 4);
	D_DrawCircle(x, y, r, v);
	return 0;
}



ELF_FUNCTION(L_DrawRectangle) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	f32 w = elf_loadnum(S, 3);
	f32 h = elf_loadnum(S, 4);

	D_DrawRectangle(x, y, w, h);
	return 0;
}



ELF_FUNCTION(L_DrawRectOutline) {
	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	f32 w = elf_loadnum(S, 3);
	f32 h = elf_loadnum(S, 4);

	D_DrawRectangle(x, y-1, w, 1);
	D_DrawRectangle(x, y+h, w, 1);
	D_DrawRectangle(x-1, y, 1, h);
	D_DrawRectangle(x+w, y, 1, h);
	return 0;
}



ELF_FUNCTION(L_GetTextureResolution) {
	RID id = (RID) elf_loadint(S, 1);
	vec2i reso = R_GetTextureInfo(gd.rend, id);

	elf_pushtab(S);

	elf_pushtext(S, "x");
	elf_pushint(S, reso.x);
	elf_setfield(S);

	elf_pushtext(S, "y");
	elf_pushint(S, reso.y);
	elf_setfield(S);
	return 1;
}



ELF_FUNCTION(L_LoadTexture) {
	const char *name = elf_loadtext(S, 1);

	vec2i resolution;
	i32 num_channels;
	i32 wanted_channels = 4;

	unsigned char *colors = stbi_load(name, &resolution.x, &resolution.y, &num_channels, wanted_channels);
	if (colors) {

		RID rid = R_InstallTexture(gd.rend, FORMAT_R8G8B8_UNORM, resolution, colors);
		elf_pushsys(S, rid);

		free(colors);
	} else {

		elf_pushnil(S);
	}
	return 1;
}



// todo: possibly rename to "Keyboard" and then MouseButton to "Mouse"
ELF_FUNCTION(L_Button) {
	int state = 0;
	for (int i = 1; i < nargs; i ++) {
		i32 index = elf_loadint(S, i);
		state |= OS_GetWindowKey(gd.window, index);
	}
	elf_pushint(S, state);
	return 1;
}



ELF_FUNCTION(L_MouseButton) {
	i32 index = elf_loadint(S, 1);
	elf_pushint(S, OS_GetWindowKey(gd.window, KEY_MOUSE_LEFT + index));
	return 1;
}



ELF_FUNCTION(L_GetMouseWheel) {
	elf_pushint(S, OS_GetWindowMouseWheel(gd.window).y);
	return 1;
}



ELF_FUNCTION(L_GetMouseX) {
	elf_pushnum(S, gd.mouse.x);
	return 1;
}



ELF_FUNCTION(L_GetMouseY) {
	elf_pushnum(S, gd.mouse.y);
	return 1;
}



static void BeginDrawing() {
	R_BeginFrame(gd.rend);

	D_SetScale(1, 1);
	D_SetOffset(0, 0);
	D_SetCenter(0, 0);
	D_LoadIdentity();
	D_SetTexture(RID_TEXTURE_DEFAULT);

	R_SetSurface(gd.rend, gd.targetsurface);
	R_SetVirtualReso(gd.rend, gd.vreso);
	R_SetSampler(gd.rend, SAMPLER_POINT);
	R_SetViewportFullScreen(gd.rend);
	R_SetTopology(gd.rend, TOPO_TRIANGLES);
	R_SetBlender(gd.rend, BLENDER_ALPHA_BLEND);

}



ELF_FUNCTION(L_InitWindow) {

	const char *name = elf_loadtext(S, 1);

	vec2i vreso = { 0 };
	vec2i wreso = { 0 };


	if (nargs >= 4) {
		vreso.x = wreso.x = elf_loadint(S, 2);
		vreso.y = wreso.y = elf_loadint(S, 3);

		if (nargs >= 5) {
			int windowscale = elf_loadint(S, 4);
			wreso.x *= windowscale;
			wreso.y *= windowscale;
		}
	}


	OS_InstallWindow(WINDOW_MAIN, name, wreso);
	TRACELOG("Init Window... %ix%i", wreso.x, wreso.y);

	R_Renderer *rend = R_InitRenderer(WINDOW_MAIN);
	gd.rend = rend;
	gd.window = WINDOW_MAIN;
	gd.targetsurface = RID_RENDER_TARGET_WINDOW;
	gd.vreso = vreso;
	TRACELOG("Init Renderer... %ix%i", vreso.x, vreso.y);
	if (vreso.x > 0 && vreso.y > 0) {
		gd.basesurface = R_InstallSurface(rend, FORMAT_R8G8B8_UNORM, vreso);
		gd.targetsurface = gd.basesurface;
	}


	gd.target_seconds_to_sleep = 1.0 / 60.0;
	gd.begin_cycle_clock = OS_GetTickCounter();

	// OS_PollWindow(window);
	BeginDrawing();
	return 0;
}

ELF_FUNCTION(L_PollWindow) {

	R_Renderer *rend = gd.rend;
	OS_WindowId window = gd.window;

	b32 open = OS_PollWindow(window);


	if (gd.targetsurface != RID_RENDER_TARGET_WINDOW) {
		R_Blit(rend, RID_RENDER_TARGET_WINDOW, gd.targetsurface);
	}
	R_Synchronize(rend);
	R_EndFrame(rend);


	if (!open) goto esc;

	// because we only have one window
	OS_ForgetFileDrops();

	// begin a new frame
	BeginDrawing();

	// todo: remove this from the renderer!
	iRect rect = R_GetBlitRect(rend, RID_RENDER_TARGET_WINDOW, gd.targetsurface);
	vec2i mouse = OS_GetWindowMouse(window);

	// can be negative
	gd.mouse.x = 0 + (mouse.x - rect.x) / (f32) rect.w;
	gd.mouse.y = 1 - (mouse.y - rect.y) / (f32) rect.h;

	// todo: fix this thing, it works too well
	i64 end_cycle_clock = OS_GetTickCounter();
	i64 elapsed_clocks = end_cycle_clock - gd.begin_cycle_clock;
	f64 elapsed_seconds = elapsed_clocks * OS_GetClocksToSeconds();
	gd.pending_seconds_to_sleep += gd.target_seconds_to_sleep - elapsed_seconds;

	// todo: instead of making this logic work in seconds
	// make it work in clock cycles directly
	f64 clock_accuracy = 1.0 / 1000.0;
	gd.begin_cycle_clock = OS_GetTickCounter();
	while (gd.pending_seconds_to_sleep > clock_accuracy) {
		OS_Sleep(gd.pending_seconds_to_sleep * 1000);
		gd.begin_cycle_clock = OS_GetTickCounter();
		gd.pending_seconds_to_sleep -= (gd.begin_cycle_clock - end_cycle_clock) * OS_GetClocksToSeconds();
	}

	esc:
	elf_pushint(S, open);
	return 1;
}



ELF_FUNCTION(L_SetFont) {
	D_SetFont((FONT_HANDLE) elf_loadsys(S, 1));
	return 0;
}



ELF_FUNCTION(L_LoadFont) {

	R_Renderer *rend = gd.rend;

	const char *name = elf_loadtext(S, 1);
	int size = elf_loadint(S, 2);

	D_FONT *font = InstallFont((char *) name, size);
	if (font) {
		elf_pushsys(S, (elf_Handle) font);
	} else {
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
	R_Renderer *rend = gd.rend;

	f32 x = elf_loadnum(S, 1);
	f32 y = elf_loadnum(S, 2);
	const char *text = elf_loadtext(S, 3);

	D_DrawText(x, y, text);
	return 0;
}



ELF_FUNCTION(L_GetFileDrop) {

	OS_WindowId window = gd.window;
	i32 index = elf_loadint(S, 1);
	elf_pushtext(S, OS_GetFileDrop(index));
	return 1;
}

ELF_FUNCTION(L_GetNumFileDrops) {

	OS_WindowId window = gd.window;
	elf_pushint(S, OS_GetNumFileDrops());
	return 1;
}

ELF_FUNCTION(L_OpenFileDialog) {

	const char *path = elf_loadtext(S, 1);

	char buffer[256] = {};
	int result = OS_OpenFileDialog((char *) path, buffer, sizeof(buffer));
	if (result) {
		elf_pushtext(S, buffer);
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
	int voiceid = elf_loadint(S, 1);
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
	int id = elf_loadint(S, 1);
	const char *name = elf_loadtext(S, 2);
	int error = A_LoadSoundFromFile(id, (char *) name);
	elf_pushint(S, error);
	return 1;
}



ELF_FUNCTION(L_PlaySound) {
	int id = elf_loadint(S, 1);
	int voiceid = A_PlaySound(id);
	elf_pushint(S, voiceid);
	return 1;
}

ELF_FUNCTION(L_StopVoice) {
	int id = elf_loadint(S, 1);
	A_StopVoice(id);
	return 0;
}


static const elf_Binding l_state[] = {
	{ "InitWindow"                       , L_InitWindow                           },
	{ "PollWindow"                       , L_PollWindow                           },
	{ "LoadTexture"                      , L_LoadTexture                          },
	{ "GetTextureResolution"             , L_GetTextureResolution                 },
	{ "SetViewport"                      , L_SetViewport                          },

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
	{ "SetLayerColor"                    , L_SetLayerColor                        },
	{ "SetLayerAlpha"                    , L_SetLayerAlpha                        },
	{ "SetAlpha"                         , L_SetAlpha                             },
	{ "SetColorP"                        , L_SetColorP                            },
	{ "SetColor"                         , L_SetColor                             },
	{ "SetColor0"                        , L_SetColor0                            },
	{ "SetColor1"                        , L_SetColor1                            },
	{ "SetColor2"                        , L_SetColor2                            },
	{ "SetColor3"                        , L_SetColor3                            },
	{ "SolidFill"                        , L_SolidFill                            },

	{ "DrawRectangle"                    , L_DrawRectangle                        },
	{ "DrawRectOutline"                  , L_DrawRectOutline                      },
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
