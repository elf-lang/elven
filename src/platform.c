#include "elements.h"
#include "platform.h"

#include <malloc.h>


#define WIN32_LEAN_AND_MEAN
#include     <windows.h>
#include     <timeapi.h>
#include     <shellapi.h>
#include     <commdlg.h>


enum {
	MAX_WINDOWS   = 8,
	MAX_FILEDROPS = 8,
};


typedef struct {
	HWND             window;
	vec2i            resolution;
	b32              closed;
	vec2i            mouse_wheel;
	vec2i            mouse;
	Button           perpollkeys[256];
	int              nextdropindex;
} MWINDOW;



typedef struct {
	WID window;
	char      name[256];
} MFILEDROP;



struct {
	f64       clocks_to_seconds;
	u8        key_remapper[256];
	// todo:
	MFILEDROP filedrops[MAX_FILEDROPS];
	int       dropindex;
} global g;



static LRESULT Win32_WindowProcedure(HWND window, UINT msg, WPARAM w, LPARAM l);


typedef BOOL win32_SetProcessDPIAwarenessContext(void* value);
typedef UINT win32_GetDPIForWindow(HWND hwnd);
#define WIN32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((void*)-4)
#define WIN32_WINDOW_CLASS_NAME L"graphical-window"


#define KEYMAPDEF(_)                          \
_(VK_NUMPAD0        , '0'              ,  10) \
_(VK_F1             , KEY_F1           ,  24) \
_('A'               , 'A'              ,  26) \
_('0'               , '0'              ,  10) \
_(VK_SPACE          , ' '              ,   1) \
_(VK_RETURN         , '\t'             ,   1) \
_(VK_UP             , KEY_UP           ,   1) \
_(VK_DOWN           , KEY_DOWN         ,   1) \
_(VK_LEFT           , KEY_LEFT         ,   1) \
_(VK_RIGHT          , KEY_RIGHT        ,   1) \
_(VK_ESCAPE         , KEY_ESCAPE       ,   1) \
/* end */



MWINDOW *WindowFromWID(WID id) {
	return (MWINDOW *) id;
}



f64 OS_GetClocksToSeconds() {
	return g.clocks_to_seconds;
}


int OS_GetWindowKey(WID id, int index) {
	return WindowFromWID(id)->perpollkeys[index].u;
}


vec2i OS_GetWindowMouseWheel(WID id) {
	return WindowFromWID(id)->mouse_wheel;
}


vec2i OS_GetWindowMouse(WID id) {
	return WindowFromWID(id)->mouse;
}


vec2i OS_GetWindowResolution(WID id) {
	return WindowFromWID(id)->resolution;
}


HWND OS_GetWindowHandle(WID id) {
	return WindowFromWID(id)->window;
}


void OS_Sleep(int ms) {
	Sleep(ms);
}


char *OS_GetFileDrop(int index) {
	return g.filedrops[index].name;
}


int OS_GetNumFileDrops() {
	return g.dropindex;
}


void OS_ForgetFileDrops() {
	g.dropindex = 0;
}


int OS_GetNextFileDrop(WID id) {
	MWINDOW *window = WindowFromWID(id);
	if (window->nextdropindex>g.dropindex){
		window->nextdropindex=0;
	}
	for (; window->nextdropindex<g.dropindex; ++window->nextdropindex) {
		if (g.filedrops[window->nextdropindex].window == id) {
			return window->nextdropindex;
		}
	}
	return -1;
}


void OS_InitPlatform() {
#define MAPPER(VKEY, MYKEY, RANGE) for (i32 i = 0; i < RANGE; i += 1) { g.key_remapper[VKEY + i] = MYKEY + i; }
	KEYMAPDEF(MAPPER)
#undef MAPPER

	timeBeginPeriod(1);

	LARGE_INTEGER i = {};
	QueryPerformanceFrequency(&i);

	f64 frequency = i.QuadPart;
	g.clocks_to_seconds = (f64) 1.0 / frequency;

	// prevent windows from using DPI scaling
	{
		HMODULE user32 = LoadLibraryA("user32.dll");
		if (user32) {
			win32_SetProcessDPIAwarenessContext *set_dpi_awareness = (win32_SetProcessDPIAwarenessContext*)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
			if (set_dpi_awareness) {
				set_dpi_awareness(WIN32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			}
			FreeLibrary(user32);
		}
	}

	{
		WNDCLASSEXW window_class = {
			.cbSize = sizeof(window_class),
			.lpfnWndProc = Win32_WindowProcedure,
			.hInstance = GetModuleHandle(0),
			.lpszClassName = WIN32_WINDOW_CLASS_NAME,
			.hCursor = LoadCursorA(NULL, IDC_ARROW),
			.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(1)),
		};
		if (!RegisterClassExW(&window_class)) {
			OS_ShowErrorMessage(0);
		}
	}
}


void OS_EndPlatform() {
	timeEndPeriod(1);
}




b32 OS_PollWindow(WID id) {
	MWINDOW *window = WindowFromWID(id);
	HWND hWindow = window->window;

	window->nextdropindex = 0;


	window->mouse_wheel.x = 0;
	window->mouse_wheel.y = 0;

	for (i32 i = 0; i < COUNTOF(window->perpollkeys); i ++) {
		window->perpollkeys[i].u &= BUTTON_DOWN;
	}

	MSG msg = {};
	while (PeekMessage(&msg, hWindow, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	RECT window_r;
	GetClientRect(hWindow, &window_r);

	window->resolution.x = window_r.right - window_r.left;
	window->resolution.y = window_r.bottom - window_r.top;

	return ! window->closed;
}



WID OS_InstallWindow(const char *name, vec2i resolution) {
	MWINDOW *window = calloc(1, sizeof(*window));


	int monitorw = GetSystemMetrics(SM_CXSCREEN);
	int monitorh = GetSystemMetrics(SM_CYSCREEN);
	int maxw = monitorw * 0.75;
	int maxh = monitorh * 0.75;

	if (resolution.x == 0) resolution.x = maxw;
	if (resolution.y == 0) resolution.y = maxh;
	int scale = MIN(maxw / resolution.x, maxh / resolution.y);
	resolution.x *= scale;
	resolution.y *= scale;

	RECT window_rect = {
		.left = 0,
		.top = 0,
		.right = resolution.x,
		.bottom = resolution.y,
	};

	b32 error = AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);
	ASSERT(error != 0);

	resolution.x = window_rect.right - window_rect.left;
	resolution.y = window_rect.bottom - window_rect.top;

	int windowx = (monitorw - resolution.x) * 0.5;
	int windowy = (monitorh - resolution.y) * 0.5;

	window->resolution = resolution;


	HWND hWindow;

	{
		wchar_t window_title[256] = {};
		MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,name,-1,window_title,sizeof(window_title));

		hWindow = CreateWindowExW(0
		, WIN32_WINDOW_CLASS_NAME, window_title
		, WS_OVERLAPPEDWINDOW, windowx, windowy, resolution.x, resolution.y
		, NULL, NULL, GetModuleHandle(0), NULL);

		SetWindowLongPtrA(hWindow, GWLP_USERDATA, (LONG_PTR) window);
	}

	if (IsWindow(hWindow)) {
		// todo: only show the window once we've got something to render!?
		ShowWindow(hWindow, SW_SHOW);
		DragAcceptFiles(hWindow, TRUE);

		window->window = hWindow;

		TRACELOG("Install Window... %ix%i", resolution.x, resolution.y);
	} else {
		TRACELOG("Install Window... Failed");

		OS_ShowErrorMessage(0);
	}

	return (WID) window;
}


i64 OS_GetTickCounter() {
	LARGE_INTEGER p = {};
	QueryPerformanceCounter(&p);
	return p.QuadPart;
}


void OS_ShowErrorMessage(char *msg) {
	if (!msg) {
		DWORD error = GetLastError();
		LPSTR message = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
		, NULL, error, 0, (LPSTR) &message, 0, NULL);
		msg = message;
	}
	MessageBoxA(NULL, msg, "Error", MB_OK | MB_ICONERROR);
}


b32 OS_OpenFileDialog(char *path, char *buffer, int bufsize) {
	b32 result = false;

	// todo:
	// why would it do this?
	char temp[MAX_PATH];
	GetCurrentDirectory(sizeof(temp),temp);

	{
		OPENFILENAMEA config = {
			.lStructSize = sizeof(OPENFILENAMEA),
			.hwndOwner = NULL,
			.lpstrFile = buffer,
			.nMaxFile = bufsize,
			.lpstrFilter = "All\0*.*\0Text\0*.TXT\0",
			.nFilterIndex = 1,
			.lpstrFileTitle = NULL,
			.nMaxFileTitle = 0,
			.lpstrInitialDir = path,
			.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
		};
		result = GetOpenFileNameA(&config);
	}
	SetCurrentDirectory(temp);

	return result;
}


static LRESULT Win32_WindowProcedure(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
	MWINDOW *window = (MWINDOW *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

	LRESULT yield = FALSE;

	switch(msg) {
		case WM_DROPFILES: {
			HDROP hDrop = (HDROP) w;

			int nFiles = DragQueryFileA(hDrop, 0xFFFFFFFF, NULL, 0);

			for (int i = g.dropindex; i < nFiles; i ++) {
				g.filedrops[i].window = (WID) window;
				DragQueryFileA(hDrop, i, g.filedrops[i].name, sizeof(g.filedrops[i].name));
				printf("Droppped File: %s\n", g.filedrops[i].name);
			}

			g.dropindex += nFiles;

			DragFinish(hDrop);
		} break;
		case WM_CLOSE: {
			TRACELOG("Close Window");

			window->closed = true;
		} break;
		case WM_ENTERSIZEMOVE:
		case WM_EXITSIZEMOVE: {
         // app->resizing = msg == WM_ENTERSIZEMOVE;
		} break;

		case WM_LBUTTONUP: case WM_LBUTTONDOWN: {
			update_button(&window->perpollkeys[KEY_MOUSE_LEFT], msg == WM_LBUTTONDOWN);
		} break;
		case WM_MBUTTONUP: case WM_MBUTTONDOWN: {
			update_button(&window->perpollkeys[KEY_MOUSE_MIDDLE], msg == WM_MBUTTONDOWN);
		} break;
		case WM_RBUTTONUP: case WM_RBUTTONDOWN: {
			update_button(&window->perpollkeys[KEY_MOUSE_RIGHT], msg == WM_RBUTTONDOWN);
		} break;

		case WM_SYSKEYUP: case WM_KEYUP: {
			update_button(&window->perpollkeys[g.key_remapper[w]], FALSE);
		} break;
		case WM_SYSKEYDOWN: case WM_KEYDOWN: {
			update_button(&window->perpollkeys[g.key_remapper[w]], TRUE);
		} break;
		case WM_MOUSEMOVE: {
			window->mouse.x = LOWORD(l);
			window->mouse.y = HIWORD(l);
		} break;
		case WM_MOUSEWHEEL: {
			window->mouse_wheel.y = GET_WHEEL_DELTA_WPARAM(w);
			window->mouse_wheel.y = window->mouse_wheel.y < 0 ? -1 : 1;
		} break;
		case WM_MOUSEHWHEEL: {
			// window->mouse_wheel.x = HIWORD(w);
		} break;
		case WM_CHAR: {
		} break;
		// case WM_SETCURSOR: {
		// 	SetCursor(os.cursor);
		// } break;
		// case WM_KILLFOCUS: {
		// 	ReleaseCapture();
		// } break;
      // case WM_DPICHANGED: {
      // } break;
		default: {
			yield = DefWindowProcW(hwnd, msg, w, l);
		} break;
	}

	return yield;
}


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
