
// todo: if it's just one value then grayscale
// todo: color modes for HSV!
static inline Color _get_color_arg(elf_State *S) {
	Color color = {};
	if (elf_get_num_args(S) > 2) {
		color.r = elf_get_int(S, 0);
		color.g = elf_get_int(S, 1);
		color.b = elf_get_int(S, 2);
		if (elf_get_num_args(S) > 3) {
			color.a = elf_get_int(S, 3);
		}
	}
	return color;
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
	jam_State *J = (jam_State *) S;

	R_Renderer *rend = J->rend;
	D_SolidFill(rend);
	return 0;
}

ELF_FUNCTION(L_SetTexture) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	TextureId id = elf_get_int(S, 0);
	D_SetTexture(rend, id);
	return 0;
}

ELF_FUNCTION(L_SetRegion) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	i32 x0 = elf_get_num(S, 0);
	i32 y0 = elf_get_num(S, 1);
	i32 x1 = x0 + elf_get_num(S, 2);
	i32 y1 = y0 + elf_get_num(S, 3);
	D_SetRegion(x0, y0, x1, y1);
	return 0;
}

ELF_FUNCTION(L_SetRotation) {
	D_SetRotation(elf_get_num(S,0));
	return 0;
}

ELF_FUNCTION(L_SetCenter) {
	f32 x = elf_get_num(S,0);
	f32 y = elf_get_num(S,1);
	D_SetCenter(x, y);
	return 0;
}

ELF_FUNCTION(L_SetScale) {
	f32 x = elf_get_num(S,0);
	f32 y = elf_get_num(S,1);
	D_SetScale(x, y);
	return 0;
}

ELF_FUNCTION(L_SetOffset) {
	f32 x = elf_get_num(S,0);
	f32 y = elf_get_num(S,1);
	D_SetOffset(x, y);
	return 0;
}

ELF_FUNCTION(L_SetFlipOnce) {
	int x = elf_get_int(S,0);
	int y = elf_get_int(S,1);
	D_SetFlipOnce(x, y);
	return 0;
}

ELF_FUNCTION(L_Clear) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	Color color = _get_color_arg(S);
	D_Clear(rend, color);
	return 0;
}

ELF_FUNCTION(L_DrawTriangle) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	f32 x0 = elf_get_num(S, 0);
	f32 y0 = elf_get_num(S, 1);

	f32 x1 = elf_get_num(S, 2);
	f32 y1 = elf_get_num(S, 3);

	f32 x2 = elf_get_num(S, 4);
	f32 y2 = elf_get_num(S, 5);

	D_DrawTriangle(rend, x0, y0, x1, y1, x2, y2);
	return 0;
}



ELF_FUNCTION(L_DrawLine) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	f32 x0 = elf_get_num(S, 0);
	f32 y0 = elf_get_num(S, 1);
	f32 x1 = elf_get_num(S, 2);
	f32 y1 = elf_get_num(S, 3);

	D_DrawLine(rend, x0, y0, x1, y1);
	return 0;
}

ELF_FUNCTION(L_DrawCircle) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	f32 x = elf_get_num(S, 0);
	f32 y = elf_get_num(S, 1);
	f32 r = elf_get_num(S, 2);
	f32 v = elf_get_num(S, 3);

	D_DrawCircle(rend, x, y, r, v);
	return 0;
}

ELF_FUNCTION(L_DrawRectangle) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	f32 x = elf_get_num(S, 0);
	f32 y = elf_get_num(S, 1);
	f32 w = elf_get_num(S, 2);
	f32 h = elf_get_num(S, 3);

	D_DrawRectangle(rend, x, y, w, h);
	return 0;
}

ELF_FUNCTION(L_GetTextureResolution) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	int id = elf_get_int(S, 0);
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
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	char *name = elf_get_text(S, 0);

	i32 c;

	vec2i resolution = {};
	Color *colors = (Color *) stbi_load(name, &resolution.x, &resolution.y, &c, 4);

	TextureId id = R_InstallTexture(rend, 0, FORMAT_R8G8B8_UNORM, resolution, colors);

	free(colors);

	elf_push_int(S, id);
	return 1;
}

ELF_FUNCTION(L_Button) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;
	OS_Window *window = J->window;

	i32 key = elf_get_int(S,0);
	elf_push_int(S, window->buttons[key].u);
	return 1;
}

ELF_FUNCTION(L_MouseButton) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;
	OS_Window *window = J->window;

	i32 index = elf_get_int(S,0);
	elf_push_int(S, window->buttons[KEY_MOUSE_LEFT + index].u);
	return 1;
}

ELF_FUNCTION(L_GetCursorX) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;
	OS_Window *window = J->window;

	elf_push_num(S, window->mouse.x / (f32) window->window_resolution.x);
	return 1;
}

ELF_FUNCTION(L_GetCursorY) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;
	OS_Window *window = J->window;

	elf_push_num(S, 1 - window->mouse.y / (f32) window->window_resolution.y);
	return 1;
}


ELF_FUNCTION(L_InitWindow) {
	jam_State *J = (jam_State *) S;

	int i = 0;
	char *name = elf_get_text(S,i++);
	int resolution_x = elf_get_int(S,i++);
	int resolution_y = elf_get_int(S,i++);
	int window_scale = elf_get_int(S,i++);

	if (window_scale <= 0) {
		window_scale = 1;
	}

	OS_Window *window = OS_CreateWindow(&g_os, name, (vec2i){ resolution_x * window_scale, resolution_y * window_scale });

	R_Renderer *rend = R_InitRenderer((Init_Renderer) { .window = window->window });
	J->rend = rend;
	J->window = window;

	R_InstallSurface(rend, TEXTURE_RT_BASE, FORMAT_R8G8B8_UNORM, (vec2i){ resolution_x, resolution_y });

	J->target_seconds_to_sleep = 1.0 / 60.0;
	J->begin_cycle_clock = OS_GetClock();

	D_BeginDrawing(rend);
	return 0;
}

ELF_FUNCTION(L_PollWindow) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;
	OS_Window *window = J->window;

	OS_PollWindow(window);

	D_EndDrawing(rend);
	D_BeginDrawing(rend);

	i64 end_cycle_clock = OS_GetClock();
	i64 elapsed_clocks = end_cycle_clock - J->begin_cycle_clock;
	f64 elapsed_seconds = elapsed_clocks * g_os.clocks_to_seconds;
	J->pending_seconds_to_sleep += J->target_seconds_to_sleep - elapsed_seconds;


	// todo: instead of making this logic work in seconds
	// make it work in clock cycles directly
	f64 clock_accuracy = 1.0 / 1000.0;
	J->begin_cycle_clock = OS_GetClock();
	while (J->pending_seconds_to_sleep > clock_accuracy) {
		OS_Sleep(J->pending_seconds_to_sleep * 1000);
		J->begin_cycle_clock = OS_GetClock();
		J->pending_seconds_to_sleep -= (J->begin_cycle_clock - end_cycle_clock) * g_os.clocks_to_seconds;
	}

	elf_push_int(S, !window->closed);
	return 1;
}

ELF_FUNCTION(L_LoadFont) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	int slot = elf_get_int(S, 0);
	char *name = elf_get_text(S, 1);
	int size = elf_get_int(S, 2);

	int id = InstallFont(rend, slot, name, size);
	elf_push_int(S, id);
	return 1;
}

ELF_FUNCTION(L_DrawText) {
	jam_State *J = (jam_State *) S;
	R_Renderer *rend = J->rend;

	f32 x = elf_get_num(S, 0);
	f32 y = elf_get_num(S, 1);
	char *text = elf_get_text(S, 2);

	D_DrawText(rend, x, y, text);
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
	{ "GetCursorX"                       , L_GetCursorX                           },
	{ "GetCursorY"                       , L_GetCursorY                           },
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
	{ "LoadFont"                         , L_LoadFont                             },
	{ "SetFlipOnce"                      , L_SetFlipOnce                          },

	{ "InitAudio"                        , L_InitAudio                            },
	{ "LoadSound"                        , L_LoadSound                            },
	{ "PlaySound"                        , L_PlaySound                            },
};
