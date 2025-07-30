
enum {
	KEY_NONE = 0,

	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
	KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
	KEY_F0, KEY_F1, KEY_F2, KEY_F3, KEY_F4,
	KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9,

	KEY_MOUSE_LEFT,
	KEY_MOUSE_MIDDLE,
	KEY_MOUSE_RIGHT,
	KEY_MOUSE_COUNT,

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



typedef void *OS_Window_Handle;

typedef struct {
	OS_Window_Handle window;

	b32    closed;

	Button buttons[256];

	vec2   mouse;
	vec2i  window_resolution;
} OS_Window;

typedef struct {
	f64    clocks_to_seconds;
} OS_State;

void OS_InitPlatform(OS_State *os);
void OS_EndPlatform(OS_State *os);
void OS_Sleep(int ms);

i32 OS_ReadEntireFile(char *name, void *memory, i32 max_bytes_to_read);

void OS_ShowErrorMessage(char *msg);

OS_Window *OS_CreateWindow(OS_State *os, char *name, vec2i resolution);

i64 OS_GetClock();

b32 OS_PollWindow(OS_Window *window);
