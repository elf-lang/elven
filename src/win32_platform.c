// #define                NOMINMAX
// #define     WIN32_LEAN_AND_MEAN
// #define    _NO_CRT_STDIO_INLINE

// #pragma comment(lib, "Comdlg32")
// #pragma comment(lib, "Xinput")
// #pragma comment(lib, "Winmm")
// #include "windows.h"
// #include <commdlg.h>
// #include <xinput.h>
// #include <timeapi.h>


// typedef BOOL win32_SetProcessDPIAwarenessContext(void* value);
// typedef UINT win32_GetDPIForWindow(HWND hwnd);
// #define WIN32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((void*)-4)

// #define WIN32_WINDOW_CLASS_NAME L"graphical-window"

// struct{
// 	win32_GetDPIForWindow *get_dpi_for_window;
// 	WPARAM app_to_w32_key[256];
// 	OS_Key w32_to_app_key[256];
// 	/* key is OS cursor */
// 	HCURSOR cursors[OS_Cursor_COUNT];
// 	HCURSOR cursor;
// 	f64 clocks_to_seconds;
// } global platform;

// #define WIN32_CURSOR_XMAP(_)               \
// _(IDC_ARROW    ,OS_Cursor_Pointer)         \
// _(IDC_IBEAM    ,OS_Cursor_IBar)            \
// _(IDC_SIZEWE   ,OS_Cursor_LeftRight)       \
// _(IDC_SIZENS   ,OS_Cursor_UpDown)          \
// _(IDC_SIZENWSE ,OS_Cursor_DownRight)       \
// _(IDC_SIZENESW ,OS_Cursor_UpRight)         \
// _(IDC_SIZEALL  ,OS_Cursor_UpDownLeftRight) \
// _(IDC_HAND     ,OS_Cursor_HandPoint)       \
// _(IDC_NO       ,OS_Cursor_Disabled)        \
// /* end */

// #define WIN32_KEY_XMAP(_)                       \
// _(VK_NUMPAD0        , '0'                ,  10) \
// _(VK_F1             , INPUT_F1           ,  24) \
// _('A'               , 'A'                ,  26) \
// _('0'               , '0'                ,  10) \
// _(VK_SPACE          , ' '                ,   1) \
// _(VK_RETURN         , '\t'               ,   1) \
// _(VK_UP             , INPUT_UP           ,   1) \
// _(VK_DOWN           , INPUT_DOWN         ,   1) \
// _(VK_LEFT           , INPUT_LEFT         ,   1) \
// _(VK_RIGHT          , INPUT_RIGHT        ,   1) \
// _(VK_ESCAPE         , INPUT_ESCAPE       ,   1) \
// /* end */


// static f32 i16_normalize(i32 x) { return x >= 0 ? x / 32767.f : x / 32768.f; }

// /* Todo: either do this once per frame, or introduce polling time-outs,
// because according to windows polling a controller that isn't hookedup
// is not good */
// static b32 os_get_controller_state(Controller_State *state, i32 player) {
// 	XINPUT_STATE xstate;
// 	i32 error = XInputGetState(player,&xstate);
// 	state->ABXY[0] = !!(xstate.Gamepad.wButtons & 0x1000);
// 	state->ABXY[1] = !!(xstate.Gamepad.wButtons & 0x2000);
// 	state->ABXY[2] = !!(xstate.Gamepad.wButtons & 0x4000);
// 	state->ABXY[3] = !!(xstate.Gamepad.wButtons & 0x8000);
// 	state->DPAD[0] = !!(xstate.Gamepad.wButtons & 0x0001);
// 	state->DPAD[1] = !!(xstate.Gamepad.wButtons & 0x0002);
// 	state->DPAD[2] = !!(xstate.Gamepad.wButtons & 0x0004);
// 	state->DPAD[3] = !!(xstate.Gamepad.wButtons & 0x0008);
// 	state->start   = !!(xstate.Gamepad.wButtons & 0x0010);
// 	state->triggers[0] = xstate.Gamepad.bLeftTrigger / 255.f;
// 	state->triggers[1] = xstate.Gamepad.bRightTrigger / 255.f;
// 	state->thumbs[0] = (vec2){i16_normalize(xstate.Gamepad.sThumbLX),i16_normalize(xstate.Gamepad.sThumbLY)};
// 	state->thumbs[1] = (vec2){i16_normalize(xstate.Gamepad.sThumbRX),i16_normalize(xstate.Gamepad.sThumbRY)};
// 	return error == ERROR_SUCCESS;
// }

// #if 0
// internal
// vec2i os_get_window_size(jam_State window) {
// 	RECT rect;
// 	GetClientRect((HWND)window,&rect);
// 	return (vec2i){rect.right - rect.left, rect.bottom - rect.top};
// }
// #endif

// i64 jam_get_main_clock() {
// 	LARGE_INTEGER p = {};
// 	QueryPerformanceCounter(&p);
// 	return p.QuadPart;
// }


// LRESULT win32_window_proc(HWND, UINT, WPARAM, LPARAM);

// b32 os_set_cursor(OS_Cursor cursor);

// internal
// void os_display_error_dialog(char *msg) {
// 	if(!msg){
// 		DWORD error = GetLastError();
// 		LPSTR message = NULL;
// 		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
// 		, NULL, error, 0, (LPSTR)&message, 0, NULL);
// 		msg = message;
// 	}
// 	MessageBoxA(NULL, msg, "Error", MB_OK | MB_ICONERROR);
// }

// internal
// void os_init() {
// #define win32_cursor_mapper(w32_name, app_name) \
// 	platform.cursors[app_name] = LoadCursorA(0, w32_name);

// 	WIN32_CURSOR_XMAP(win32_cursor_mapper)

// #undef win32_cursor_mapper

// #define win32_key_mapper(w32_key, app_key, range_size) \
// 	for (i32 i = 0; i < range_size; i += 1) { \
// 		platform.w32_to_app_key[w32_key + i] = app_key + i; \
// 		platform.app_to_w32_key[app_key + i] = w32_key + i; \
// 	}

// 	WIN32_KEY_XMAP(win32_key_mapper)

// #undef win32_key_mapper

// 	HMODULE user32 = LoadLibraryA("user32.dll");
// 	if (user32) {
// 		win32_SetProcessDPIAwarenessContext *set_dpi_awareness = (win32_SetProcessDPIAwarenessContext*)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
// 		if (set_dpi_awareness) {
// 			set_dpi_awareness(WIN32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
// 		}
// 		platform.get_dpi_for_window = (win32_GetDPIForWindow*) GetProcAddress(user32, "GetDpiForWindow");
// 		FreeLibrary(user32);
// 	}

// #if 0
// 	SetProcessDPIAware();
// 	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
// #endif

//  	/* register graphical window class, for use when creating windows */
// 	{
// 		WNDCLASSEXW window_class = { sizeof(window_class) };
// 		window_class.lpfnWndProc = win32_window_proc;
// 		window_class.hInstance = GetModuleHandle(0);
// 		window_class.lpszClassName = WIN32_WINDOW_CLASS_NAME;
// 		window_class.hCursor = platform.cursors[OS_Cursor_Pointer];
// 		window_class.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(1));
// 		if (!RegisterClassExW(&window_class)) {
// 			os_display_error_dialog(0);
// 		}
// 	}

// 	// DEVMODEW mode = {};
// 	// if(EnumDisplaySettingsW(0, ENUM_CURRENT_SETTINGS, &mode)) {
// 	// 	platform.display_frequency = (f32) mode.dmDisplayFrequency;
// 	// }

// 	os_set_cursor(OS_Cursor_Pointer);

// 	timeBeginPeriod(1);
// 	LARGE_INTEGER large = {};
// 	QueryPerformanceFrequency(&large);
// 	f64 frequency = large.QuadPart;
// 	platform.clocks_to_seconds = (f64) 1.0 / frequency;
// 	int DEBUGBREAK;
// 	DEBUGBREAK = 0;
// }

// void os_sleep(f64 seconds) {
// 	Sleep(seconds * 1000.0);
// }


// typedef struct {
// 	char *name;
// 	int size_x;
// 	int size_y;
// } Equip_Window;
// // this is just to create a compile time dependency to
// // enforce ordering...
// typedef struct {
// 	int success;
// } Window_Token;

// static void _jam_os_cycle(jam_State *app);

// Window_Token equip_window(jam_State *app, Equip_Window params) {

// 	i32 w = GetSystemMetrics(SM_CXSCREEN);
// 	i32 h = GetSystemMetrics(SM_CYSCREEN);
// 	if (params.size_x != 0) w = params.size_x;
// 	if (params.size_y != 0) h = params.size_y;

// 	wchar_t window_title[256];
// 	MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,params.name,-1,window_title,sizeof(window_title));

// 	HWND window = CreateWindowExW(0
// 	, WIN32_WINDOW_CLASS_NAME, window_title
// 	, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h
// 	, NULL, NULL, GetModuleHandle(0), NULL);

// 	if(!IsWindow(window)){
// 		os_display_error_dialog(0);
// 	}

// 	ShowWindow(window,SW_SHOW);
// 	SetWindowLongPtrA(window,GWLP_USERDATA,(LONG_PTR)app);
// 	app->window = window;
// 	_jam_os_cycle(app);

// 	return (Window_Token) { true };
// }


// static void _jam_os_cycle(jam_State *app) {
// 	RECT rect;
// 	GetClientRect(app->window,&rect);
// 	app->window_dimensions.x = rect.right - rect.left;
// 	app->window_dimensions.y = rect.bottom - rect.top;

// 	CopyMemory(app->was_input,app->now_input,sizeof(app->now_input));

// 	MSG msg = {};
// 	while(PeekMessage(&msg,app->window,0,0,PM_REMOVE)) {
// 		TranslateMessage(&msg);
// 		DispatchMessageW(&msg);
// 	}
// }

// internal
// LRESULT win32_window_proc(HWND window, UINT msg, WPARAM w, LPARAM l) {
// 	LRESULT yield = FALSE;
// 	jam_State *app = (jam_State *)GetWindowLongPtr(window,GWLP_USERDATA);

// #define _win32_on(KEY) (app->now_input[(KEY)] = 1)
// #define _win32_off(KEY) (app->now_input[(KEY)] = 0)


// 	switch(msg) {
// 		case WM_ENTERSIZEMOVE:
// 		case WM_EXITSIZEMOVE: {
// 			app->resizing = msg == WM_ENTERSIZEMOVE;
// 		} break;
// 		case WM_CLOSE: {
// 			_win32_on(INPUT_CLOSE);
// 		} break;
// 		case WM_LBUTTONUP:   { _win32_off(INPUT_MOUSE_LEFT);   } break;
// 		case WM_MBUTTONUP:   { _win32_off(INPUT_MOUSE_MIDDLE); } break;
// 		case WM_RBUTTONUP:   { _win32_off(INPUT_MOUSE_RIGHT);  } break;
// 		case WM_LBUTTONDOWN: { _win32_on(INPUT_MOUSE_LEFT);    } break;
// 		case WM_MBUTTONDOWN: { _win32_on(INPUT_MOUSE_MIDDLE);  } break;
// 		case WM_RBUTTONDOWN: { _win32_on(INPUT_MOUSE_RIGHT);   } break;
// 		case WM_SYSKEYUP:
// 		case WM_KEYUP: {
// 			_win32_off(platform.w32_to_app_key[w]);
// 		} break;
// 		case WM_SYSKEYDOWN:
// 		case WM_KEYDOWN: {
// 			_win32_on(platform.w32_to_app_key[w]);
// 		} break;
// 		case WM_MOUSEMOVE: {
// 			app->mouse_xy.x = LOWORD(l);
// 			app->mouse_xy.y = HIWORD(l);
// 		} break;
// 		case WM_MOUSEWHEEL: {
// 			app->mouse_wheel.y = GET_WHEEL_DELTA_WPARAM(w);
// 			app->mouse_wheel.y = app->mouse_wheel.y < 0 ? -1 : 1;
// 		} break;
// 		case WM_MOUSEHWHEEL: {
// 			app->mouse_wheel.x = HIWORD(w);
// 		} break;

// 		case WM_CHAR: {
// 			// _win32_on(INPUT_Char);
// 			// if(w >= 32 && w < 127) {
// 			// 	app->character = w;
// 			// }
// 		} break;
// 		case WM_KILLFOCUS: {
// 			ReleaseCapture();
// 		} break;
// 		case WM_SETCURSOR: {
// 			/* "update" cursor */
// 			SetCursor(platform.cursor);
// 		} break;
//       // case WM_DPICHANGED: {
//       // } break;
// 		default: {
// 			yield = DefWindowProcW(window, msg, w, l);
// 		} break;
// 	}

// 	return yield;
// }

// #if 1
// b32 os_set_cursor(OS_Cursor cursor) {
// 	HCURSOR win32_cursor = platform.cursors[cursor];
// 	int resizing = false; // !platform.resizing
// 	if(win32_cursor != 0 && !resizing && win32_cursor != platform.cursor) {
// 		PostMessage(0, WM_SETCURSOR, 0, 0);
// 		POINT p = {0};
// 		GetCursorPos(&p);
// 		SetCursorPos(p.x, p.y);
// 		platform.cursor = win32_cursor;
// 	}
// 	return win32_cursor != 0;
// }
// #endif

// internal
// b32 os_open_file_dialog(char *buffer, i32 size) {
// 	b32 result = false;

// 	/* Todo: can this be prevented */
// 	char temp[MAX_PATH];
// 	GetCurrentDirectory(sizeof(temp),temp);
// 	{
// 		OPENFILENAMEA config = {};
// 		config.lStructSize     = sizeof(config);
// 		config.hwndOwner       = NULL;
// 		config.lpstrFile       = buffer;
// 		config.nMaxFile        = size;
// 		config.lpstrFilter     = "All\0*.*\0Text\0*.TXT\0";
// 		config.nFilterIndex    = 1;
// 		config.lpstrFileTitle  = NULL;
// 		config.nMaxFileTitle   = 0;
// 		config.lpstrInitialDir = NULL;
// 		config.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
// 		result = GetOpenFileNameA(&config);
// 	}
// 	SetCurrentDirectory(temp);

// 	return result;
// }