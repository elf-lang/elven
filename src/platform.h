
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



typedef u64 WID;



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


// todo: elf already has this!
i64 OS_GetTickCounter();
f64 OS_GetClocksToSeconds();


void OS_Sleep(int ms);


void OS_ShowErrorMessage(char *text);
b32 OS_OpenFileDialog(char *path, char *buf, int zbuf);


void OS_InitPlatform();
void OS_EndPlatform();


WID OS_InstallWindow(const char *name, vec2i resolution);



#if defined(_WIN32_API)
// expose this for the renderer
HWND OS_GetWindowHandle(WID window);
#endif




b32 OS_PollWindow(WID window);
// the following functions return state that is updated
// once the window is polled
b32 OS_ShouldWindowClose(WID window);
vec2i OS_GetWindowResolution(WID window);
int OS_GetWindowKey(WID window, int index);
vec2i OS_GetWindowMouse(WID window);
vec2i OS_GetWindowMouseWheel(WID window);
int OS_GetNextFileDrop(WID id);



// this is for all windows
char *OS_GetFileDrop(int index);
int OS_GetNumFileDrops();
// reset file drop counter, do once all windows are polled.
void OS_ForgetFileDrops();



