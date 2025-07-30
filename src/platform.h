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

i32 OS_ReadEntireFile(char *name, void *memory, i32 max_bytes_to_read);

void OS_ShowErrorMessage(char *msg);

OS_Window *OS_CreateWindow(OS_State *os, char *name, vec2i resolution);

i64 OS_GetClock();

b32 OS_PollWindow(OS_Window *window);
