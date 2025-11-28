

#define DOWN_SHIFT     0
#define PRESSED_SHIFT  1
#define RELEASED_SHIFT 2
#define REPEAT_SHIFT   3
enum {
	BUTTON_DOWN     = 1 << DOWN_SHIFT,
	BUTTON_PRESSED  = 1 << PRESSED_SHIFT,
	BUTTON_RELEASED = 1 << RELEASED_SHIFT,
	BUTTON_REPEAT   = 1 << REPEAT_SHIFT,
};

typedef u8 Button;

static inline Button NewButton(Button b, int d) {
	int r_bit = d << REPEAT_SHIFT;
	int d_bit = d << DOWN_SHIFT;
	int p_bit = (b ^ d & 1) << (RELEASED_SHIFT - d);
	return p_bit | d_bit | r_bit;
}



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



enum {
	OS_WINDOW_OPEN = 1,
	OS_WINDOW_VISIBLE = 2,
};

// pretend these are all readonly from the outside in...
typedef struct {
	u32       status;
	vec2i     resolution;
	vec2i     mouse_wheel;
	vec2i     mouse;
	Button    keys[256];
} OS_Window;




typedef OS_Window *WID;



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
i64 OS_GetTicksPerSecond();


void OS_Sleep(int ms);

void OS_ShowWindow(WID id);

void OS_ShowErrorMessage(char *text);
b32 OS_OpenFileDialog(char *path, char *buf, int zbuf);


void OS_InitPlatform();
void OS_EndPlatform();


WID OS_InstallWindow(const char *name, vec2i resolution);


#define OS_GET_WINDOW_HANDLE_API HWND OS_GetWindowHandle(WID window);




b32 OS_PollWindow(WID window);

#define OS_GetWindowResolution(w) ((w)->resolution)
#define OS_GetWindowKey(w, i) ((w)->keys[i])
#define OS_GetWindowMouse(w) ((w)->mouse)
#define OS_GetWindowMouseWheel(w) ((w)->mouse_wheel)

// this is for all windows
char *OS_GetFileDrop(int index);
int OS_GetNumFileDrops();
// reset file drop counter, do once all windows are polled.
void OS_ForgetFileDrops();


int OS_GetNextFileDrop(WID id);


