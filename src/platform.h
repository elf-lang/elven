
enum {
	KEY_NONE = 0,

	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
	KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
	KEY_F0, KEY_F1, KEY_F2, KEY_F3, KEY_F4,
	KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9,

	KEY_MOUSE_LEFT,
	KEY_MOUSE_RIGHT,
	KEY_MOUSE_MIDDLE,

	KEY_MENU,
	KEY_BACK,
	KEY_DEBUG,

	KEY_LEFT,
	KEY_UP,
	KEY_RIGHT,
	KEY_DOWN,

	KEY_DPAD_LEFT,
	KEY_DPAD_UP,
	KEY_DPAD_RIGHT,
	KEY_DPAD_DOWN,


	KEY_ESCAPE,

	KEY_A = 'A',
	KEY_Z = 'Z',

	KEY_COUNT = 256,
};

typedef enum {
	WINDOW_MAIN = 0,
} OS_WindowId;


typedef struct Controller_State Controller_State;
struct Controller_State {
	b32 shoulders[2];
	b32 start;
	b32 back;
	b32 DPAD[4];
	b32 ABXY[4];
	b32 bthumbs[2];
	f32 triggers[2];
	vec2 thumbs[2];
};


i32 OS_ReadEntireFile(char *name, void *memory, i32 max_bytes_to_read);
void OS_Sleep(int ms);

void OS_ShowErrorMessage(char *msg);
b32 OS_OpenFileDialog(char *path, char *buffer, int bufsize);

i64 OS_GetClock();
f64 OS_GetClocksToSeconds();

void OS_InitPlatform();
void OS_EndPlatform();


void OS_InstallWindow(OS_WindowId id, char *name, vec2i resolution);

#if defined(_WIN32_API)
HWND OS_GetWindowHandle(OS_WindowId window);
#endif


b32 OS_PollWindow(OS_WindowId window);
// the following functions return state that is updated
// once the window is polled
b32 OS_ShouldWindowClose(OS_WindowId window);
vec2i OS_GetWindowResolution(OS_WindowId window);
int OS_GetWindowKey(OS_WindowId window, int index);
vec2i OS_GetWindowMouse(OS_WindowId window);
vec2i OS_GetWindowMouseWheel(OS_WindowId window);
int OS_GetNextFileDrop(OS_WindowId id);



// this is for all windows
char *OS_GetFileDrop(int index);
int OS_GetNumFileDrops();
// reset file drop counter, do once all windows are polled.
void OS_ForgetFileDrops();



