int L_InitWindow(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	const char * title = GetText(S, 2);
	InitWindow(width, height, title);
	return result;
}
int L_CloseWindow(elf_State *S) {
	int result = 0;
	CloseWindow();
	return result;
}
int L_WindowShouldClose(elf_State *S) {
	int result = 0;
	WindowShouldClose();
	return result;
}
int L_IsWindowReady(elf_State *S) {
	int result = 0;
	IsWindowReady();
	return result;
}
int L_IsWindowFullscreen(elf_State *S) {
	int result = 0;
	IsWindowFullscreen();
	return result;
}
int L_IsWindowHidden(elf_State *S) {
	int result = 0;
	IsWindowHidden();
	return result;
}
int L_IsWindowMinimized(elf_State *S) {
	int result = 0;
	IsWindowMinimized();
	return result;
}
int L_IsWindowMaximized(elf_State *S) {
	int result = 0;
	IsWindowMaximized();
	return result;
}
int L_IsWindowFocused(elf_State *S) {
	int result = 0;
	IsWindowFocused();
	return result;
}
int L_IsWindowResized(elf_State *S) {
	int result = 0;
	IsWindowResized();
	return result;
}
int L_IsWindowState(elf_State *S) {
	int result = 0;
	unsigned int flag = GetInteger(S, 0);
	IsWindowState(flag);
	return result;
}
int L_SetWindowState(elf_State *S) {
	int result = 0;
	unsigned int flags = GetInteger(S, 0);
	SetWindowState(flags);
	return result;
}
int L_ClearWindowState(elf_State *S) {
	int result = 0;
	unsigned int flags = GetInteger(S, 0);
	ClearWindowState(flags);
	return result;
}
int L_ToggleFullscreen(elf_State *S) {
	int result = 0;
	ToggleFullscreen();
	return result;
}
int L_ToggleBorderlessWindowed(elf_State *S) {
	int result = 0;
	ToggleBorderlessWindowed();
	return result;
}
int L_MaximizeWindow(elf_State *S) {
	int result = 0;
	MaximizeWindow();
	return result;
}
int L_MinimizeWindow(elf_State *S) {
	int result = 0;
	MinimizeWindow();
	return result;
}
int L_RestoreWindow(elf_State *S) {
	int result = 0;
	RestoreWindow();
	return result;
}
int L_SetWindowIcon(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	SetWindowIcon(image);
	return result;
}
int L_SetWindowIcons(elf_State *S) {
	int result = 0;
	Image * images = undefined(S, 0);
	int count = GetInteger(S, 1);
	SetWindowIcons(images, count);
	return result;
}
int L_SetWindowTitle(elf_State *S) {
	int result = 0;
	const char * title = GetText(S, 0);
	SetWindowTitle(title);
	return result;
}
int L_SetWindowPosition(elf_State *S) {
	int result = 0;
	int x = GetInteger(S, 0);
	int y = GetInteger(S, 1);
	SetWindowPosition(x, y);
	return result;
}
int L_SetWindowMonitor(elf_State *S) {
	int result = 0;
	int monitor = GetInteger(S, 0);
	SetWindowMonitor(monitor);
	return result;
}
int L_SetWindowMinSize(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	SetWindowMinSize(width, height);
	return result;
}
int L_SetWindowMaxSize(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	SetWindowMaxSize(width, height);
	return result;
}
int L_SetWindowSize(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	SetWindowSize(width, height);
	return result;
}
int L_SetWindowOpacity(elf_State *S) {
	int result = 0;
	float opacity = GetNumber(S, 0);
	SetWindowOpacity(opacity);
	return result;
}
int L_SetWindowFocused(elf_State *S) {
	int result = 0;
	SetWindowFocused();
	return result;
}
int L_GetWindowHandle(elf_State *S) {
	int result = 0;
	GetWindowHandle();
	return result;
}
int L_GetScreenWidth(elf_State *S) {
	int result = 0;
	GetScreenWidth();
	return result;
}
int L_GetScreenHeight(elf_State *S) {
	int result = 0;
	GetScreenHeight();
	return result;
}
int L_GetRenderWidth(elf_State *S) {
	int result = 0;
	GetRenderWidth();
	return result;
}
int L_GetRenderHeight(elf_State *S) {
	int result = 0;
	GetRenderHeight();
	return result;
}
int L_GetMonitorCount(elf_State *S) {
	int result = 0;
	GetMonitorCount();
	return result;
}
int L_GetCurrentMonitor(elf_State *S) {
	int result = 0;
	GetCurrentMonitor();
	return result;
}
int L_GetMonitorPosition(elf_State *S) {
	int result = 0;
	int monitor = GetInteger(S, 0);
	GetMonitorPosition(monitor);
	return result;
}
int L_GetMonitorWidth(elf_State *S) {
	int result = 0;
	int monitor = GetInteger(S, 0);
	GetMonitorWidth(monitor);
	return result;
}
int L_GetMonitorHeight(elf_State *S) {
	int result = 0;
	int monitor = GetInteger(S, 0);
	GetMonitorHeight(monitor);
	return result;
}
int L_GetMonitorPhysicalWidth(elf_State *S) {
	int result = 0;
	int monitor = GetInteger(S, 0);
	GetMonitorPhysicalWidth(monitor);
	return result;
}
int L_GetMonitorPhysicalHeight(elf_State *S) {
	int result = 0;
	int monitor = GetInteger(S, 0);
	GetMonitorPhysicalHeight(monitor);
	return result;
}
int L_GetMonitorRefreshRate(elf_State *S) {
	int result = 0;
	int monitor = GetInteger(S, 0);
	GetMonitorRefreshRate(monitor);
	return result;
}
int L_GetWindowPosition(elf_State *S) {
	int result = 0;
	GetWindowPosition();
	return result;
}
int L_GetWindowScaleDPI(elf_State *S) {
	int result = 0;
	GetWindowScaleDPI();
	return result;
}
int L_GetMonitorName(elf_State *S) {
	int result = 0;
	int monitor = GetInteger(S, 0);
	GetMonitorName(monitor);
	return result;
}
int L_SetClipboardText(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	SetClipboardText(text);
	return result;
}
int L_GetClipboardText(elf_State *S) {
	int result = 0;
	GetClipboardText();
	return result;
}
int L_EnableEventWaiting(elf_State *S) {
	int result = 0;
	EnableEventWaiting();
	return result;
}
int L_DisableEventWaiting(elf_State *S) {
	int result = 0;
	DisableEventWaiting();
	return result;
}
int L_ShowCursor(elf_State *S) {
	int result = 0;
	ShowCursor();
	return result;
}
int L_HideCursor(elf_State *S) {
	int result = 0;
	HideCursor();
	return result;
}
int L_IsCursorHidden(elf_State *S) {
	int result = 0;
	IsCursorHidden();
	return result;
}
int L_EnableCursor(elf_State *S) {
	int result = 0;
	EnableCursor();
	return result;
}
int L_DisableCursor(elf_State *S) {
	int result = 0;
	DisableCursor();
	return result;
}
int L_IsCursorOnScreen(elf_State *S) {
	int result = 0;
	IsCursorOnScreen();
	return result;
}
int L_ClearBackground(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	ClearBackground(color);
	return result;
}
int L_BeginDrawing(elf_State *S) {
	int result = 0;
	BeginDrawing();
	return result;
}
int L_EndDrawing(elf_State *S) {
	int result = 0;
	EndDrawing();
	return result;
}
int L_BeginMode2D(elf_State *S) {
	int result = 0;
	Camera2D camera = undefined(S, 0);
	BeginMode2D(camera);
	return result;
}
int L_EndMode2D(elf_State *S) {
	int result = 0;
	EndMode2D();
	return result;
}
int L_BeginMode3D(elf_State *S) {
	int result = 0;
	Camera3D camera = undefined(S, 0);
	BeginMode3D(camera);
	return result;
}
int L_EndMode3D(elf_State *S) {
	int result = 0;
	EndMode3D();
	return result;
}
int L_BeginTextureMode(elf_State *S) {
	int result = 0;
	RenderTexture2D target = undefined(S, 0);
	BeginTextureMode(target);
	return result;
}
int L_EndTextureMode(elf_State *S) {
	int result = 0;
	EndTextureMode();
	return result;
}
int L_BeginShaderMode(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	BeginShaderMode(shader);
	return result;
}
int L_EndShaderMode(elf_State *S) {
	int result = 0;
	EndShaderMode();
	return result;
}
int L_BeginBlendMode(elf_State *S) {
	int result = 0;
	int mode = GetInteger(S, 0);
	BeginBlendMode(mode);
	return result;
}
int L_EndBlendMode(elf_State *S) {
	int result = 0;
	EndBlendMode();
	return result;
}
int L_BeginScissorMode(elf_State *S) {
	int result = 0;
	int x = GetInteger(S, 0);
	int y = GetInteger(S, 1);
	int width = GetInteger(S, 2);
	int height = GetInteger(S, 3);
	BeginScissorMode(x, y, width, height);
	return result;
}
int L_EndScissorMode(elf_State *S) {
	int result = 0;
	EndScissorMode();
	return result;
}
int L_BeginVrStereoMode(elf_State *S) {
	int result = 0;
	VrStereoConfig config = undefined(S, 0);
	BeginVrStereoMode(config);
	return result;
}
int L_EndVrStereoMode(elf_State *S) {
	int result = 0;
	EndVrStereoMode();
	return result;
}
int L_LoadVrStereoConfig(elf_State *S) {
	int result = 0;
	VrDeviceInfo device = undefined(S, 0);
	LoadVrStereoConfig(device);
	return result;
}
int L_UnloadVrStereoConfig(elf_State *S) {
	int result = 0;
	VrStereoConfig config = undefined(S, 0);
	UnloadVrStereoConfig(config);
	return result;
}
int L_LoadShader(elf_State *S) {
	int result = 0;
	const char * vsFileName = GetText(S, 0);
	const char * fsFileName = GetText(S, 1);
	LoadShader(vsFileName, fsFileName);
	return result;
}
int L_LoadShaderFromMemory(elf_State *S) {
	int result = 0;
	const char * vsCode = GetText(S, 0);
	const char * fsCode = GetText(S, 1);
	LoadShaderFromMemory(vsCode, fsCode);
	return result;
}
int L_IsShaderReady(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	IsShaderReady(shader);
	return result;
}
int L_GetShaderLocation(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	const char * uniformName = GetText(S, 1);
	GetShaderLocation(shader, uniformName);
	return result;
}
int L_GetShaderLocationAttrib(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	const char * attribName = GetText(S, 1);
	GetShaderLocationAttrib(shader, attribName);
	return result;
}
int L_SetShaderValue(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	int locIndex = GetInteger(S, 1);
	const void * value = undefined(S, 2);
	int uniformType = GetInteger(S, 3);
	SetShaderValue(shader, locIndex, value, uniformType);
	return result;
}
int L_SetShaderValueV(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	int locIndex = GetInteger(S, 1);
	const void * value = undefined(S, 2);
	int uniformType = GetInteger(S, 3);
	int count = GetInteger(S, 4);
	SetShaderValueV(shader, locIndex, value, uniformType, count);
	return result;
}
int L_SetShaderValueMatrix(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	int locIndex = GetInteger(S, 1);
	Matrix mat = undefined(S, 2);
	SetShaderValueMatrix(shader, locIndex, mat);
	return result;
}
int L_SetShaderValueTexture(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	int locIndex = GetInteger(S, 1);
	Texture2D texture = undefined(S, 2);
	SetShaderValueTexture(shader, locIndex, texture);
	return result;
}
int L_UnloadShader(elf_State *S) {
	int result = 0;
	Shader shader = undefined(S, 0);
	UnloadShader(shader);
	return result;
}
int L_GetScreenToWorldRay(elf_State *S) {
	int result = 0;
	Vector2 position = undefined(S, 0);
	Camera camera = undefined(S, 1);
	GetScreenToWorldRay(position, camera);
	return result;
}
int L_GetScreenToWorldRayEx(elf_State *S) {
	int result = 0;
	Vector2 position = undefined(S, 0);
	Camera camera = undefined(S, 1);
	int width = GetInteger(S, 2);
	int height = GetInteger(S, 3);
	GetScreenToWorldRayEx(position, camera, width, height);
	return result;
}
int L_GetWorldToScreen(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	Camera camera = undefined(S, 1);
	GetWorldToScreen(position, camera);
	return result;
}
int L_GetWorldToScreenEx(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	Camera camera = undefined(S, 1);
	int width = GetInteger(S, 2);
	int height = GetInteger(S, 3);
	GetWorldToScreenEx(position, camera, width, height);
	return result;
}
int L_GetWorldToScreen2D(elf_State *S) {
	int result = 0;
	Vector2 position = undefined(S, 0);
	Camera2D camera = undefined(S, 1);
	GetWorldToScreen2D(position, camera);
	return result;
}
int L_GetScreenToWorld2D(elf_State *S) {
	int result = 0;
	Vector2 position = undefined(S, 0);
	Camera2D camera = undefined(S, 1);
	GetScreenToWorld2D(position, camera);
	return result;
}
int L_GetCameraMatrix(elf_State *S) {
	int result = 0;
	Camera camera = undefined(S, 0);
	GetCameraMatrix(camera);
	return result;
}
int L_GetCameraMatrix2D(elf_State *S) {
	int result = 0;
	Camera2D camera = undefined(S, 0);
	GetCameraMatrix2D(camera);
	return result;
}
int L_SetTargetFPS(elf_State *S) {
	int result = 0;
	int fps = GetInteger(S, 0);
	SetTargetFPS(fps);
	return result;
}
int L_GetFrameTime(elf_State *S) {
	int result = 0;
	GetFrameTime();
	return result;
}
int L_GetTime(elf_State *S) {
	int result = 0;
	GetTime();
	return result;
}
int L_GetFPS(elf_State *S) {
	int result = 0;
	GetFPS();
	return result;
}
int L_SwapScreenBuffer(elf_State *S) {
	int result = 0;
	SwapScreenBuffer();
	return result;
}
int L_PollInputEvents(elf_State *S) {
	int result = 0;
	PollInputEvents();
	return result;
}
int L_WaitTime(elf_State *S) {
	int result = 0;
	double seconds = GetNumber(S, 0);
	WaitTime(seconds);
	return result;
}
int L_SetRandomSeed(elf_State *S) {
	int result = 0;
	unsigned int seed = GetInteger(S, 0);
	SetRandomSeed(seed);
	return result;
}
int L_GetRandomValue(elf_State *S) {
	int result = 0;
	int min = GetInteger(S, 0);
	int max = GetInteger(S, 1);
	GetRandomValue(min, max);
	return result;
}
int L_LoadRandomSequence(elf_State *S) {
	int result = 0;
	unsigned int count = GetInteger(S, 0);
	int min = GetInteger(S, 1);
	int max = GetInteger(S, 2);
	LoadRandomSequence(count, min, max);
	return result;
}
int L_UnloadRandomSequence(elf_State *S) {
	int result = 0;
	int * sequence = undefined(S, 0);
	UnloadRandomSequence(sequence);
	return result;
}
int L_TakeScreenshot(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	TakeScreenshot(fileName);
	return result;
}
int L_SetConfigFlags(elf_State *S) {
	int result = 0;
	unsigned int flags = GetInteger(S, 0);
	SetConfigFlags(flags);
	return result;
}
int L_OpenURL(elf_State *S) {
	int result = 0;
	const char * url = GetText(S, 0);
	OpenURL(url);
	return result;
}
int L_TraceLog(elf_State *S) {
	int result = 0;
	int logLevel = GetInteger(S, 0);
	const char * text = GetText(S, 1);
	... args = undefined(S, 2);
	TraceLog(logLevel, text, args);
	return result;
}
int L_SetTraceLogLevel(elf_State *S) {
	int result = 0;
	int logLevel = GetInteger(S, 0);
	SetTraceLogLevel(logLevel);
	return result;
}
int L_MemAlloc(elf_State *S) {
	int result = 0;
	unsigned int size = GetInteger(S, 0);
	MemAlloc(size);
	return result;
}
int L_MemRealloc(elf_State *S) {
	int result = 0;
	void * ptr = undefined(S, 0);
	unsigned int size = GetInteger(S, 1);
	MemRealloc(ptr, size);
	return result;
}
int L_MemFree(elf_State *S) {
	int result = 0;
	void * ptr = undefined(S, 0);
	MemFree(ptr);
	return result;
}
int L_SetTraceLogCallback(elf_State *S) {
	int result = 0;
	TraceLogCallback callback = undefined(S, 0);
	SetTraceLogCallback(callback);
	return result;
}
int L_SetLoadFileDataCallback(elf_State *S) {
	int result = 0;
	LoadFileDataCallback callback = undefined(S, 0);
	SetLoadFileDataCallback(callback);
	return result;
}
int L_SetSaveFileDataCallback(elf_State *S) {
	int result = 0;
	SaveFileDataCallback callback = undefined(S, 0);
	SetSaveFileDataCallback(callback);
	return result;
}
int L_SetLoadFileTextCallback(elf_State *S) {
	int result = 0;
	LoadFileTextCallback callback = undefined(S, 0);
	SetLoadFileTextCallback(callback);
	return result;
}
int L_SetSaveFileTextCallback(elf_State *S) {
	int result = 0;
	SaveFileTextCallback callback = undefined(S, 0);
	SetSaveFileTextCallback(callback);
	return result;
}
int L_LoadFileData(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	int * dataSize = undefined(S, 1);
	LoadFileData(fileName, dataSize);
	return result;
}
int L_UnloadFileData(elf_State *S) {
	int result = 0;
	unsigned char * data = undefined(S, 0);
	UnloadFileData(data);
	return result;
}
int L_SaveFileData(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	void * data = undefined(S, 1);
	int dataSize = GetInteger(S, 2);
	SaveFileData(fileName, data, dataSize);
	return result;
}
int L_ExportDataAsCode(elf_State *S) {
	int result = 0;
	const unsigned char * data = undefined(S, 0);
	int dataSize = GetInteger(S, 1);
	const char * fileName = GetText(S, 2);
	ExportDataAsCode(data, dataSize, fileName);
	return result;
}
int L_LoadFileText(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadFileText(fileName);
	return result;
}
int L_UnloadFileText(elf_State *S) {
	int result = 0;
	char * text = GetText(S, 0);
	UnloadFileText(text);
	return result;
}
int L_SaveFileText(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	char * text = GetText(S, 1);
	SaveFileText(fileName, text);
	return result;
}
int L_FileExists(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	FileExists(fileName);
	return result;
}
int L_DirectoryExists(elf_State *S) {
	int result = 0;
	const char * dirPath = GetText(S, 0);
	DirectoryExists(dirPath);
	return result;
}
int L_IsFileExtension(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	const char * ext = GetText(S, 1);
	IsFileExtension(fileName, ext);
	return result;
}
int L_GetFileLength(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	GetFileLength(fileName);
	return result;
}
int L_GetFileExtension(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	GetFileExtension(fileName);
	return result;
}
int L_GetFileName(elf_State *S) {
	int result = 0;
	const char * filePath = GetText(S, 0);
	GetFileName(filePath);
	return result;
}
int L_GetFileNameWithoutExt(elf_State *S) {
	int result = 0;
	const char * filePath = GetText(S, 0);
	GetFileNameWithoutExt(filePath);
	return result;
}
int L_GetDirectoryPath(elf_State *S) {
	int result = 0;
	const char * filePath = GetText(S, 0);
	GetDirectoryPath(filePath);
	return result;
}
int L_GetPrevDirectoryPath(elf_State *S) {
	int result = 0;
	const char * dirPath = GetText(S, 0);
	GetPrevDirectoryPath(dirPath);
	return result;
}
int L_GetWorkingDirectory(elf_State *S) {
	int result = 0;
	GetWorkingDirectory();
	return result;
}
int L_GetApplicationDirectory(elf_State *S) {
	int result = 0;
	GetApplicationDirectory();
	return result;
}
int L_ChangeDirectory(elf_State *S) {
	int result = 0;
	const char * dir = GetText(S, 0);
	ChangeDirectory(dir);
	return result;
}
int L_IsPathFile(elf_State *S) {
	int result = 0;
	const char * path = GetText(S, 0);
	IsPathFile(path);
	return result;
}
int L_LoadDirectoryFiles(elf_State *S) {
	int result = 0;
	const char * dirPath = GetText(S, 0);
	LoadDirectoryFiles(dirPath);
	return result;
}
int L_LoadDirectoryFilesEx(elf_State *S) {
	int result = 0;
	const char * basePath = GetText(S, 0);
	const char * filter = GetText(S, 1);
	bool scanSubdirs = GetInteger(S, 2);
	LoadDirectoryFilesEx(basePath, filter, scanSubdirs);
	return result;
}
int L_UnloadDirectoryFiles(elf_State *S) {
	int result = 0;
	FilePathList files = undefined(S, 0);
	UnloadDirectoryFiles(files);
	return result;
}
int L_IsFileDropped(elf_State *S) {
	int result = 0;
	IsFileDropped();
	return result;
}
int L_LoadDroppedFiles(elf_State *S) {
	int result = 0;
	LoadDroppedFiles();
	return result;
}
int L_UnloadDroppedFiles(elf_State *S) {
	int result = 0;
	FilePathList files = undefined(S, 0);
	UnloadDroppedFiles(files);
	return result;
}
int L_GetFileModTime(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	GetFileModTime(fileName);
	return result;
}
int L_CompressData(elf_State *S) {
	int result = 0;
	const unsigned char * data = undefined(S, 0);
	int dataSize = GetInteger(S, 1);
	int * compDataSize = undefined(S, 2);
	CompressData(data, dataSize, compDataSize);
	return result;
}
int L_DecompressData(elf_State *S) {
	int result = 0;
	const unsigned char * compData = undefined(S, 0);
	int compDataSize = GetInteger(S, 1);
	int * dataSize = undefined(S, 2);
	DecompressData(compData, compDataSize, dataSize);
	return result;
}
int L_EncodeDataBase64(elf_State *S) {
	int result = 0;
	const unsigned char * data = undefined(S, 0);
	int dataSize = GetInteger(S, 1);
	int * outputSize = undefined(S, 2);
	EncodeDataBase64(data, dataSize, outputSize);
	return result;
}
int L_DecodeDataBase64(elf_State *S) {
	int result = 0;
	const unsigned char * data = undefined(S, 0);
	int * outputSize = undefined(S, 1);
	DecodeDataBase64(data, outputSize);
	return result;
}
int L_LoadAutomationEventList(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadAutomationEventList(fileName);
	return result;
}
int L_UnloadAutomationEventList(elf_State *S) {
	int result = 0;
	AutomationEventList list = undefined(S, 0);
	UnloadAutomationEventList(list);
	return result;
}
int L_ExportAutomationEventList(elf_State *S) {
	int result = 0;
	AutomationEventList list = undefined(S, 0);
	const char * fileName = GetText(S, 1);
	ExportAutomationEventList(list, fileName);
	return result;
}
int L_SetAutomationEventList(elf_State *S) {
	int result = 0;
	AutomationEventList * list = undefined(S, 0);
	SetAutomationEventList(list);
	return result;
}
int L_SetAutomationEventBaseFrame(elf_State *S) {
	int result = 0;
	int frame = GetInteger(S, 0);
	SetAutomationEventBaseFrame(frame);
	return result;
}
int L_StartAutomationEventRecording(elf_State *S) {
	int result = 0;
	StartAutomationEventRecording();
	return result;
}
int L_StopAutomationEventRecording(elf_State *S) {
	int result = 0;
	StopAutomationEventRecording();
	return result;
}
int L_PlayAutomationEvent(elf_State *S) {
	int result = 0;
	AutomationEvent event = undefined(S, 0);
	PlayAutomationEvent(event);
	return result;
}
int L_IsKeyPressed(elf_State *S) {
	int result = 0;
	int key = GetInteger(S, 0);
	IsKeyPressed(key);
	return result;
}
int L_IsKeyPressedRepeat(elf_State *S) {
	int result = 0;
	int key = GetInteger(S, 0);
	IsKeyPressedRepeat(key);
	return result;
}
int L_IsKeyDown(elf_State *S) {
	int result = 0;
	int key = GetInteger(S, 0);
	IsKeyDown(key);
	return result;
}
int L_IsKeyReleased(elf_State *S) {
	int result = 0;
	int key = GetInteger(S, 0);
	IsKeyReleased(key);
	return result;
}
int L_IsKeyUp(elf_State *S) {
	int result = 0;
	int key = GetInteger(S, 0);
	IsKeyUp(key);
	return result;
}
int L_GetKeyPressed(elf_State *S) {
	int result = 0;
	GetKeyPressed();
	return result;
}
int L_GetCharPressed(elf_State *S) {
	int result = 0;
	GetCharPressed();
	return result;
}
int L_SetExitKey(elf_State *S) {
	int result = 0;
	int key = GetInteger(S, 0);
	SetExitKey(key);
	return result;
}
int L_IsGamepadAvailable(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	IsGamepadAvailable(gamepad);
	return result;
}
int L_GetGamepadName(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	GetGamepadName(gamepad);
	return result;
}
int L_IsGamepadButtonPressed(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	int button = GetInteger(S, 1);
	IsGamepadButtonPressed(gamepad, button);
	return result;
}
int L_IsGamepadButtonDown(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	int button = GetInteger(S, 1);
	IsGamepadButtonDown(gamepad, button);
	return result;
}
int L_IsGamepadButtonReleased(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	int button = GetInteger(S, 1);
	IsGamepadButtonReleased(gamepad, button);
	return result;
}
int L_IsGamepadButtonUp(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	int button = GetInteger(S, 1);
	IsGamepadButtonUp(gamepad, button);
	return result;
}
int L_GetGamepadButtonPressed(elf_State *S) {
	int result = 0;
	GetGamepadButtonPressed();
	return result;
}
int L_GetGamepadAxisCount(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	GetGamepadAxisCount(gamepad);
	return result;
}
int L_GetGamepadAxisMovement(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	int axis = GetInteger(S, 1);
	GetGamepadAxisMovement(gamepad, axis);
	return result;
}
int L_SetGamepadMappings(elf_State *S) {
	int result = 0;
	const char * mappings = GetText(S, 0);
	SetGamepadMappings(mappings);
	return result;
}
int L_SetGamepadVibration(elf_State *S) {
	int result = 0;
	int gamepad = GetInteger(S, 0);
	float leftMotor = GetNumber(S, 1);
	float rightMotor = GetNumber(S, 2);
	SetGamepadVibration(gamepad, leftMotor, rightMotor);
	return result;
}
int L_IsMouseButtonPressed(elf_State *S) {
	int result = 0;
	int button = GetInteger(S, 0);
	IsMouseButtonPressed(button);
	return result;
}
int L_IsMouseButtonDown(elf_State *S) {
	int result = 0;
	int button = GetInteger(S, 0);
	IsMouseButtonDown(button);
	return result;
}
int L_IsMouseButtonReleased(elf_State *S) {
	int result = 0;
	int button = GetInteger(S, 0);
	IsMouseButtonReleased(button);
	return result;
}
int L_IsMouseButtonUp(elf_State *S) {
	int result = 0;
	int button = GetInteger(S, 0);
	IsMouseButtonUp(button);
	return result;
}
int L_GetMouseX(elf_State *S) {
	int result = 0;
	GetMouseX();
	return result;
}
int L_GetMouseY(elf_State *S) {
	int result = 0;
	GetMouseY();
	return result;
}
int L_GetMousePosition(elf_State *S) {
	int result = 0;
	GetMousePosition();
	return result;
}
int L_GetMouseDelta(elf_State *S) {
	int result = 0;
	GetMouseDelta();
	return result;
}
int L_SetMousePosition(elf_State *S) {
	int result = 0;
	int x = GetInteger(S, 0);
	int y = GetInteger(S, 1);
	SetMousePosition(x, y);
	return result;
}
int L_SetMouseOffset(elf_State *S) {
	int result = 0;
	int offsetX = GetInteger(S, 0);
	int offsetY = GetInteger(S, 1);
	SetMouseOffset(offsetX, offsetY);
	return result;
}
int L_SetMouseScale(elf_State *S) {
	int result = 0;
	float scaleX = GetNumber(S, 0);
	float scaleY = GetNumber(S, 1);
	SetMouseScale(scaleX, scaleY);
	return result;
}
int L_GetMouseWheelMove(elf_State *S) {
	int result = 0;
	GetMouseWheelMove();
	return result;
}
int L_GetMouseWheelMoveV(elf_State *S) {
	int result = 0;
	GetMouseWheelMoveV();
	return result;
}
int L_SetMouseCursor(elf_State *S) {
	int result = 0;
	int cursor = GetInteger(S, 0);
	SetMouseCursor(cursor);
	return result;
}
int L_GetTouchX(elf_State *S) {
	int result = 0;
	GetTouchX();
	return result;
}
int L_GetTouchY(elf_State *S) {
	int result = 0;
	GetTouchY();
	return result;
}
int L_GetTouchPosition(elf_State *S) {
	int result = 0;
	int index = GetInteger(S, 0);
	GetTouchPosition(index);
	return result;
}
int L_GetTouchPointId(elf_State *S) {
	int result = 0;
	int index = GetInteger(S, 0);
	GetTouchPointId(index);
	return result;
}
int L_GetTouchPointCount(elf_State *S) {
	int result = 0;
	GetTouchPointCount();
	return result;
}
int L_SetGesturesEnabled(elf_State *S) {
	int result = 0;
	unsigned int flags = GetInteger(S, 0);
	SetGesturesEnabled(flags);
	return result;
}
int L_IsGestureDetected(elf_State *S) {
	int result = 0;
	unsigned int gesture = GetInteger(S, 0);
	IsGestureDetected(gesture);
	return result;
}
int L_GetGestureDetected(elf_State *S) {
	int result = 0;
	GetGestureDetected();
	return result;
}
int L_GetGestureHoldDuration(elf_State *S) {
	int result = 0;
	GetGestureHoldDuration();
	return result;
}
int L_GetGestureDragVector(elf_State *S) {
	int result = 0;
	GetGestureDragVector();
	return result;
}
int L_GetGestureDragAngle(elf_State *S) {
	int result = 0;
	GetGestureDragAngle();
	return result;
}
int L_GetGesturePinchVector(elf_State *S) {
	int result = 0;
	GetGesturePinchVector();
	return result;
}
int L_GetGesturePinchAngle(elf_State *S) {
	int result = 0;
	GetGesturePinchAngle();
	return result;
}
int L_UpdateCamera(elf_State *S) {
	int result = 0;
	Camera * camera = undefined(S, 0);
	int mode = GetInteger(S, 1);
	UpdateCamera(camera, mode);
	return result;
}
int L_UpdateCameraPro(elf_State *S) {
	int result = 0;
	Camera * camera = undefined(S, 0);
	Vector3 movement = undefined(S, 1);
	Vector3 rotation = undefined(S, 2);
	float zoom = GetNumber(S, 3);
	UpdateCameraPro(camera, movement, rotation, zoom);
	return result;
}
int L_SetShapesTexture(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	Rect source = undefined(S, 1);
	SetShapesTexture(texture, source);
	return result;
}
int L_GetShapesTexture(elf_State *S) {
	int result = 0;
	GetShapesTexture();
	return result;
}
int L_GetShapesTextureRectangle(elf_State *S) {
	int result = 0;
	GetShapesTextureRectangle();
	return result;
}
int L_DrawPixel(elf_State *S) {
	int result = 0;
	int posX = GetInteger(S, 0);
	int posY = GetInteger(S, 1);
	Color color = undefined(S, 2);
	DrawPixel(posX, posY, color);
	return result;
}
int L_DrawPixelV(elf_State *S) {
	int result = 0;
	Vector2 position = undefined(S, 0);
	Color color = undefined(S, 1);
	DrawPixelV(position, color);
	return result;
}
int L_DrawLine(elf_State *S) {
	int result = 0;
	int startPosX = GetInteger(S, 0);
	int startPosY = GetInteger(S, 1);
	int endPosX = GetInteger(S, 2);
	int endPosY = GetInteger(S, 3);
	Color color = undefined(S, 4);
	DrawLine(startPosX, startPosY, endPosX, endPosY, color);
	return result;
}
int L_DrawLineV(elf_State *S) {
	int result = 0;
	Vector2 startPos = undefined(S, 0);
	Vector2 endPos = undefined(S, 1);
	Color color = undefined(S, 2);
	DrawLineV(startPos, endPos, color);
	return result;
}
int L_DrawLineEx(elf_State *S) {
	int result = 0;
	Vector2 startPos = undefined(S, 0);
	Vector2 endPos = undefined(S, 1);
	float thick = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawLineEx(startPos, endPos, thick, color);
	return result;
}
int L_DrawLineStrip(elf_State *S) {
	int result = 0;
	Vector2 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	Color color = undefined(S, 2);
	DrawLineStrip(points, pointCount, color);
	return result;
}
int L_DrawLineBezier(elf_State *S) {
	int result = 0;
	Vector2 startPos = undefined(S, 0);
	Vector2 endPos = undefined(S, 1);
	float thick = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawLineBezier(startPos, endPos, thick, color);
	return result;
}
int L_DrawCircle(elf_State *S) {
	int result = 0;
	int centerX = GetInteger(S, 0);
	int centerY = GetInteger(S, 1);
	float radius = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawCircle(centerX, centerY, radius, color);
	return result;
}
int L_DrawCircleSector(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	float radius = GetNumber(S, 1);
	float startAngle = GetNumber(S, 2);
	float endAngle = GetNumber(S, 3);
	int segments = GetInteger(S, 4);
	Color color = undefined(S, 5);
	DrawCircleSector(center, radius, startAngle, endAngle, segments, color);
	return result;
}
int L_DrawCircleSectorLines(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	float radius = GetNumber(S, 1);
	float startAngle = GetNumber(S, 2);
	float endAngle = GetNumber(S, 3);
	int segments = GetInteger(S, 4);
	Color color = undefined(S, 5);
	DrawCircleSectorLines(center, radius, startAngle, endAngle, segments, color);
	return result;
}
int L_DrawCircleGradient(elf_State *S) {
	int result = 0;
	int centerX = GetInteger(S, 0);
	int centerY = GetInteger(S, 1);
	float radius = GetNumber(S, 2);
	Color color1 = undefined(S, 3);
	Color color2 = undefined(S, 4);
	DrawCircleGradient(centerX, centerY, radius, color1, color2);
	return result;
}
int L_DrawCircleV(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	float radius = GetNumber(S, 1);
	Color color = undefined(S, 2);
	DrawCircleV(center, radius, color);
	return result;
}
int L_DrawCircleLines(elf_State *S) {
	int result = 0;
	int centerX = GetInteger(S, 0);
	int centerY = GetInteger(S, 1);
	float radius = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawCircleLines(centerX, centerY, radius, color);
	return result;
}
int L_DrawCircleLinesV(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	float radius = GetNumber(S, 1);
	Color color = undefined(S, 2);
	DrawCircleLinesV(center, radius, color);
	return result;
}
int L_DrawEllipse(elf_State *S) {
	int result = 0;
	int centerX = GetInteger(S, 0);
	int centerY = GetInteger(S, 1);
	float radiusH = GetNumber(S, 2);
	float radiusV = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawEllipse(centerX, centerY, radiusH, radiusV, color);
	return result;
}
int L_DrawEllipseLines(elf_State *S) {
	int result = 0;
	int centerX = GetInteger(S, 0);
	int centerY = GetInteger(S, 1);
	float radiusH = GetNumber(S, 2);
	float radiusV = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
	return result;
}
int L_DrawRing(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	float innerRadius = GetNumber(S, 1);
	float outerRadius = GetNumber(S, 2);
	float startAngle = GetNumber(S, 3);
	float endAngle = GetNumber(S, 4);
	int segments = GetInteger(S, 5);
	Color color = undefined(S, 6);
	DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
	return result;
}
int L_DrawRingLines(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	float innerRadius = GetNumber(S, 1);
	float outerRadius = GetNumber(S, 2);
	float startAngle = GetNumber(S, 3);
	float endAngle = GetNumber(S, 4);
	int segments = GetInteger(S, 5);
	Color color = undefined(S, 6);
	DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
	return result;
}
int L_DrawRectangle(elf_State *S) {
	int result = 0;
	int posX = GetInteger(S, 0);
	int posY = GetInteger(S, 1);
	int width = GetInteger(S, 2);
	int height = GetInteger(S, 3);
	Color color = undefined(S, 4);
	DrawRectangle(posX, posY, width, height, color);
	return result;
}
int L_DrawRectangleV(elf_State *S) {
	int result = 0;
	Vector2 position = undefined(S, 0);
	Vector2 size = undefined(S, 1);
	Color color = undefined(S, 2);
	DrawRectangleV(position, size, color);
	return result;
}
int L_DrawRectangleRec(elf_State *S) {
	int result = 0;
	Rect rec = undefined(S, 0);
	Color color = undefined(S, 1);
	DrawRectangleRec(rec, color);
	return result;
}
int L_DrawRectanglePro(elf_State *S) {
	int result = 0;
	Rect rec = undefined(S, 0);
	Vector2 origin = undefined(S, 1);
	float rotation = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawRectanglePro(rec, origin, rotation, color);
	return result;
}
int L_DrawRectangleGradientV(elf_State *S) {
	int result = 0;
	int posX = GetInteger(S, 0);
	int posY = GetInteger(S, 1);
	int width = GetInteger(S, 2);
	int height = GetInteger(S, 3);
	Color color1 = undefined(S, 4);
	Color color2 = undefined(S, 5);
	DrawRectangleGradientV(posX, posY, width, height, color1, color2);
	return result;
}
int L_DrawRectangleGradientH(elf_State *S) {
	int result = 0;
	int posX = GetInteger(S, 0);
	int posY = GetInteger(S, 1);
	int width = GetInteger(S, 2);
	int height = GetInteger(S, 3);
	Color color1 = undefined(S, 4);
	Color color2 = undefined(S, 5);
	DrawRectangleGradientH(posX, posY, width, height, color1, color2);
	return result;
}
int L_DrawRectangleGradientEx(elf_State *S) {
	int result = 0;
	Rect rec = undefined(S, 0);
	Color col1 = undefined(S, 1);
	Color col2 = undefined(S, 2);
	Color col3 = undefined(S, 3);
	Color col4 = undefined(S, 4);
	DrawRectangleGradientEx(rec, col1, col2, col3, col4);
	return result;
}
int L_DrawRectangleLines(elf_State *S) {
	int result = 0;
	int posX = GetInteger(S, 0);
	int posY = GetInteger(S, 1);
	int width = GetInteger(S, 2);
	int height = GetInteger(S, 3);
	Color color = undefined(S, 4);
	DrawRectangleLines(posX, posY, width, height, color);
	return result;
}
int L_DrawRectangleLinesEx(elf_State *S) {
	int result = 0;
	Rect rec = undefined(S, 0);
	float lineThick = GetNumber(S, 1);
	Color color = undefined(S, 2);
	DrawRectangleLinesEx(rec, lineThick, color);
	return result;
}
int L_DrawRectangleRounded(elf_State *S) {
	int result = 0;
	Rect rec = undefined(S, 0);
	float roundness = GetNumber(S, 1);
	int segments = GetInteger(S, 2);
	Color color = undefined(S, 3);
	DrawRectangleRounded(rec, roundness, segments, color);
	return result;
}
int L_DrawRectangleRoundedLines(elf_State *S) {
	int result = 0;
	Rect rec = undefined(S, 0);
	float roundness = GetNumber(S, 1);
	int segments = GetInteger(S, 2);
	Color color = undefined(S, 3);
	DrawRectangleRoundedLines(rec, roundness, segments, color);
	return result;
}
int L_DrawRectangleRoundedLinesEx(elf_State *S) {
	int result = 0;
	Rect rec = undefined(S, 0);
	float roundness = GetNumber(S, 1);
	int segments = GetInteger(S, 2);
	float lineThick = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawRectangleRoundedLinesEx(rec, roundness, segments, lineThick, color);
	return result;
}
int L_DrawTriangle(elf_State *S) {
	int result = 0;
	Vector2 v1 = undefined(S, 0);
	Vector2 v2 = undefined(S, 1);
	Vector2 v3 = undefined(S, 2);
	Color color = undefined(S, 3);
	DrawTriangle(v1, v2, v3, color);
	return result;
}
int L_DrawTriangleLines(elf_State *S) {
	int result = 0;
	Vector2 v1 = undefined(S, 0);
	Vector2 v2 = undefined(S, 1);
	Vector2 v3 = undefined(S, 2);
	Color color = undefined(S, 3);
	DrawTriangleLines(v1, v2, v3, color);
	return result;
}
int L_DrawTriangleFan(elf_State *S) {
	int result = 0;
	Vector2 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	Color color = undefined(S, 2);
	DrawTriangleFan(points, pointCount, color);
	return result;
}
int L_DrawTriangleStrip(elf_State *S) {
	int result = 0;
	Vector2 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	Color color = undefined(S, 2);
	DrawTriangleStrip(points, pointCount, color);
	return result;
}
int L_DrawPoly(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	int sides = GetInteger(S, 1);
	float radius = GetNumber(S, 2);
	float rotation = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawPoly(center, sides, radius, rotation, color);
	return result;
}
int L_DrawPolyLines(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	int sides = GetInteger(S, 1);
	float radius = GetNumber(S, 2);
	float rotation = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawPolyLines(center, sides, radius, rotation, color);
	return result;
}
int L_DrawPolyLinesEx(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	int sides = GetInteger(S, 1);
	float radius = GetNumber(S, 2);
	float rotation = GetNumber(S, 3);
	float lineThick = GetNumber(S, 4);
	Color color = undefined(S, 5);
	DrawPolyLinesEx(center, sides, radius, rotation, lineThick, color);
	return result;
}
int L_DrawSplineLinear(elf_State *S) {
	int result = 0;
	Vector2 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	float thick = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawSplineLinear(points, pointCount, thick, color);
	return result;
}
int L_DrawSplineBasis(elf_State *S) {
	int result = 0;
	Vector2 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	float thick = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawSplineBasis(points, pointCount, thick, color);
	return result;
}
int L_DrawSplineCatmullRom(elf_State *S) {
	int result = 0;
	Vector2 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	float thick = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawSplineCatmullRom(points, pointCount, thick, color);
	return result;
}
int L_DrawSplineBezierQuadratic(elf_State *S) {
	int result = 0;
	Vector2 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	float thick = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawSplineBezierQuadratic(points, pointCount, thick, color);
	return result;
}
int L_DrawSplineBezierCubic(elf_State *S) {
	int result = 0;
	Vector2 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	float thick = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawSplineBezierCubic(points, pointCount, thick, color);
	return result;
}
int L_DrawSplineSegmentLinear(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 p2 = undefined(S, 1);
	float thick = GetNumber(S, 2);
	Color color = undefined(S, 3);
	DrawSplineSegmentLinear(p1, p2, thick, color);
	return result;
}
int L_DrawSplineSegmentBasis(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 p2 = undefined(S, 1);
	Vector2 p3 = undefined(S, 2);
	Vector2 p4 = undefined(S, 3);
	float thick = GetNumber(S, 4);
	Color color = undefined(S, 5);
	DrawSplineSegmentBasis(p1, p2, p3, p4, thick, color);
	return result;
}
int L_DrawSplineSegmentCatmullRom(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 p2 = undefined(S, 1);
	Vector2 p3 = undefined(S, 2);
	Vector2 p4 = undefined(S, 3);
	float thick = GetNumber(S, 4);
	Color color = undefined(S, 5);
	DrawSplineSegmentCatmullRom(p1, p2, p3, p4, thick, color);
	return result;
}
int L_DrawSplineSegmentBezierQuadratic(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 c2 = undefined(S, 1);
	Vector2 p3 = undefined(S, 2);
	float thick = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawSplineSegmentBezierQuadratic(p1, c2, p3, thick, color);
	return result;
}
int L_DrawSplineSegmentBezierCubic(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 c2 = undefined(S, 1);
	Vector2 c3 = undefined(S, 2);
	Vector2 p4 = undefined(S, 3);
	float thick = GetNumber(S, 4);
	Color color = undefined(S, 5);
	DrawSplineSegmentBezierCubic(p1, c2, c3, p4, thick, color);
	return result;
}
int L_GetSplinePointLinear(elf_State *S) {
	int result = 0;
	Vector2 startPos = undefined(S, 0);
	Vector2 endPos = undefined(S, 1);
	float t = GetNumber(S, 2);
	GetSplinePointLinear(startPos, endPos, t);
	return result;
}
int L_GetSplinePointBasis(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 p2 = undefined(S, 1);
	Vector2 p3 = undefined(S, 2);
	Vector2 p4 = undefined(S, 3);
	float t = GetNumber(S, 4);
	GetSplinePointBasis(p1, p2, p3, p4, t);
	return result;
}
int L_GetSplinePointCatmullRom(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 p2 = undefined(S, 1);
	Vector2 p3 = undefined(S, 2);
	Vector2 p4 = undefined(S, 3);
	float t = GetNumber(S, 4);
	GetSplinePointCatmullRom(p1, p2, p3, p4, t);
	return result;
}
int L_GetSplinePointBezierQuad(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 c2 = undefined(S, 1);
	Vector2 p3 = undefined(S, 2);
	float t = GetNumber(S, 3);
	GetSplinePointBezierQuad(p1, c2, p3, t);
	return result;
}
int L_GetSplinePointBezierCubic(elf_State *S) {
	int result = 0;
	Vector2 p1 = undefined(S, 0);
	Vector2 c2 = undefined(S, 1);
	Vector2 c3 = undefined(S, 2);
	Vector2 p4 = undefined(S, 3);
	float t = GetNumber(S, 4);
	GetSplinePointBezierCubic(p1, c2, c3, p4, t);
	return result;
}
int L_CheckCollisionRecs(elf_State *S) {
	int result = 0;
	Rect rec1 = undefined(S, 0);
	Rect rec2 = undefined(S, 1);
	CheckCollisionRecs(rec1, rec2);
	return result;
}
int L_CheckCollisionCircles(elf_State *S) {
	int result = 0;
	Vector2 center1 = undefined(S, 0);
	float radius1 = GetNumber(S, 1);
	Vector2 center2 = undefined(S, 2);
	float radius2 = GetNumber(S, 3);
	CheckCollisionCircles(center1, radius1, center2, radius2);
	return result;
}
int L_CheckCollisionCircleRec(elf_State *S) {
	int result = 0;
	Vector2 center = undefined(S, 0);
	float radius = GetNumber(S, 1);
	Rect rec = undefined(S, 2);
	CheckCollisionCircleRec(center, radius, rec);
	return result;
}
int L_CheckCollisionPointRec(elf_State *S) {
	int result = 0;
	Vector2 point = undefined(S, 0);
	Rect rec = undefined(S, 1);
	CheckCollisionPointRec(point, rec);
	return result;
}
int L_CheckCollisionPointCircle(elf_State *S) {
	int result = 0;
	Vector2 point = undefined(S, 0);
	Vector2 center = undefined(S, 1);
	float radius = GetNumber(S, 2);
	CheckCollisionPointCircle(point, center, radius);
	return result;
}
int L_CheckCollisionPointTriangle(elf_State *S) {
	int result = 0;
	Vector2 point = undefined(S, 0);
	Vector2 p1 = undefined(S, 1);
	Vector2 p2 = undefined(S, 2);
	Vector2 p3 = undefined(S, 3);
	CheckCollisionPointTriangle(point, p1, p2, p3);
	return result;
}
int L_CheckCollisionPointPoly(elf_State *S) {
	int result = 0;
	Vector2 point = undefined(S, 0);
	Vector2 * points = undefined(S, 1);
	int pointCount = GetInteger(S, 2);
	CheckCollisionPointPoly(point, points, pointCount);
	return result;
}
int L_CheckCollisionLines(elf_State *S) {
	int result = 0;
	Vector2 startPos1 = undefined(S, 0);
	Vector2 endPos1 = undefined(S, 1);
	Vector2 startPos2 = undefined(S, 2);
	Vector2 endPos2 = undefined(S, 3);
	Vector2 * collisionPoint = undefined(S, 4);
	CheckCollisionLines(startPos1, endPos1, startPos2, endPos2, collisionPoint);
	return result;
}
int L_CheckCollisionPointLine(elf_State *S) {
	int result = 0;
	Vector2 point = undefined(S, 0);
	Vector2 p1 = undefined(S, 1);
	Vector2 p2 = undefined(S, 2);
	int threshold = GetInteger(S, 3);
	CheckCollisionPointLine(point, p1, p2, threshold);
	return result;
}
int L_GetCollisionRec(elf_State *S) {
	int result = 0;
	Rect rec1 = undefined(S, 0);
	Rect rec2 = undefined(S, 1);
	GetCollisionRec(rec1, rec2);
	return result;
}
int L_LoadImage(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadImage(fileName);
	return result;
}
int L_LoadImageRaw(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	int width = GetInteger(S, 1);
	int height = GetInteger(S, 2);
	int format = GetInteger(S, 3);
	int headerSize = GetInteger(S, 4);
	LoadImageRaw(fileName, width, height, format, headerSize);
	return result;
}
int L_LoadImageSvg(elf_State *S) {
	int result = 0;
	const char * fileNameOrString = GetText(S, 0);
	int width = GetInteger(S, 1);
	int height = GetInteger(S, 2);
	LoadImageSvg(fileNameOrString, width, height);
	return result;
}
int L_LoadImageAnim(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	int * frames = undefined(S, 1);
	LoadImageAnim(fileName, frames);
	return result;
}
int L_LoadImageAnimFromMemory(elf_State *S) {
	int result = 0;
	const char * fileType = GetText(S, 0);
	const unsigned char * fileData = undefined(S, 1);
	int dataSize = GetInteger(S, 2);
	int * frames = undefined(S, 3);
	LoadImageAnimFromMemory(fileType, fileData, dataSize, frames);
	return result;
}
int L_LoadImageFromMemory(elf_State *S) {
	int result = 0;
	const char * fileType = GetText(S, 0);
	const unsigned char * fileData = undefined(S, 1);
	int dataSize = GetInteger(S, 2);
	LoadImageFromMemory(fileType, fileData, dataSize);
	return result;
}
int L_LoadImageFromTexture(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	LoadImageFromTexture(texture);
	return result;
}
int L_LoadImageFromScreen(elf_State *S) {
	int result = 0;
	LoadImageFromScreen();
	return result;
}
int L_IsImageReady(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	IsImageReady(image);
	return result;
}
int L_UnloadImage(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	UnloadImage(image);
	return result;
}
int L_ExportImage(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	const char * fileName = GetText(S, 1);
	ExportImage(image, fileName);
	return result;
}
int L_ExportImageToMemory(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	const char * fileType = GetText(S, 1);
	int * fileSize = undefined(S, 2);
	ExportImageToMemory(image, fileType, fileSize);
	return result;
}
int L_ExportImageAsCode(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	const char * fileName = GetText(S, 1);
	ExportImageAsCode(image, fileName);
	return result;
}
int L_GenImageColor(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	Color color = undefined(S, 2);
	GenImageColor(width, height, color);
	return result;
}
int L_GenImageGradientLinear(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	int direction = GetInteger(S, 2);
	Color start = undefined(S, 3);
	Color end = undefined(S, 4);
	GenImageGradientLinear(width, height, direction, start, end);
	return result;
}
int L_GenImageGradientRadial(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	float density = GetNumber(S, 2);
	Color inner = undefined(S, 3);
	Color outer = undefined(S, 4);
	GenImageGradientRadial(width, height, density, inner, outer);
	return result;
}
int L_GenImageGradientSquare(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	float density = GetNumber(S, 2);
	Color inner = undefined(S, 3);
	Color outer = undefined(S, 4);
	GenImageGradientSquare(width, height, density, inner, outer);
	return result;
}
int L_GenImageChecked(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	int checksX = GetInteger(S, 2);
	int checksY = GetInteger(S, 3);
	Color col1 = undefined(S, 4);
	Color col2 = undefined(S, 5);
	GenImageChecked(width, height, checksX, checksY, col1, col2);
	return result;
}
int L_GenImageWhiteNoise(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	float factor = GetNumber(S, 2);
	GenImageWhiteNoise(width, height, factor);
	return result;
}
int L_GenImagePerlinNoise(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	int offsetX = GetInteger(S, 2);
	int offsetY = GetInteger(S, 3);
	float scale = GetNumber(S, 4);
	GenImagePerlinNoise(width, height, offsetX, offsetY, scale);
	return result;
}
int L_GenImageCellular(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	int tileSize = GetInteger(S, 2);
	GenImageCellular(width, height, tileSize);
	return result;
}
int L_GenImageText(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	const char * text = GetText(S, 2);
	GenImageText(width, height, text);
	return result;
}
int L_ImageCopy(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	ImageCopy(image);
	return result;
}
int L_ImageFromImage(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	Rect rec = undefined(S, 1);
	ImageFromImage(image, rec);
	return result;
}
int L_ImageText(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	int fontSize = GetInteger(S, 1);
	Color color = undefined(S, 2);
	ImageText(text, fontSize, color);
	return result;
}
int L_ImageTextEx(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	const char * text = GetText(S, 1);
	float fontSize = GetNumber(S, 2);
	float spacing = GetNumber(S, 3);
	Color tint = undefined(S, 4);
	ImageTextEx(font, text, fontSize, spacing, tint);
	return result;
}
int L_ImageFormat(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	int newFormat = GetInteger(S, 1);
	ImageFormat(image, newFormat);
	return result;
}
int L_ImageToPOT(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	Color fill = undefined(S, 1);
	ImageToPOT(image, fill);
	return result;
}
int L_ImageCrop(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	Rect crop = undefined(S, 1);
	ImageCrop(image, crop);
	return result;
}
int L_ImageAlphaCrop(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	float threshold = GetNumber(S, 1);
	ImageAlphaCrop(image, threshold);
	return result;
}
int L_ImageAlphaClear(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	Color color = undefined(S, 1);
	float threshold = GetNumber(S, 2);
	ImageAlphaClear(image, color, threshold);
	return result;
}
int L_ImageAlphaMask(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	Image alphaMask = undefined(S, 1);
	ImageAlphaMask(image, alphaMask);
	return result;
}
int L_ImageAlphaPremultiply(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	ImageAlphaPremultiply(image);
	return result;
}
int L_ImageBlurGaussian(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	int blurSize = GetInteger(S, 1);
	ImageBlurGaussian(image, blurSize);
	return result;
}
int L_ImageKernelConvolution(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	float* kernel = undefined(S, 1);
	int kernelSize = GetInteger(S, 2);
	ImageKernelConvolution(image, kernel, kernelSize);
	return result;
}
int L_ImageResize(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	int newWidth = GetInteger(S, 1);
	int newHeight = GetInteger(S, 2);
	ImageResize(image, newWidth, newHeight);
	return result;
}
int L_ImageResizeNN(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	int newWidth = GetInteger(S, 1);
	int newHeight = GetInteger(S, 2);
	ImageResizeNN(image, newWidth, newHeight);
	return result;
}
int L_ImageResizeCanvas(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	int newWidth = GetInteger(S, 1);
	int newHeight = GetInteger(S, 2);
	int offsetX = GetInteger(S, 3);
	int offsetY = GetInteger(S, 4);
	Color fill = undefined(S, 5);
	ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, fill);
	return result;
}
int L_ImageMipmaps(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	ImageMipmaps(image);
	return result;
}
int L_ImageDither(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	int rBpp = GetInteger(S, 1);
	int gBpp = GetInteger(S, 2);
	int bBpp = GetInteger(S, 3);
	int aBpp = GetInteger(S, 4);
	ImageDither(image, rBpp, gBpp, bBpp, aBpp);
	return result;
}
int L_ImageFlipVertical(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	ImageFlipVertical(image);
	return result;
}
int L_ImageFlipHorizontal(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	ImageFlipHorizontal(image);
	return result;
}
int L_ImageRotate(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	int degrees = GetInteger(S, 1);
	ImageRotate(image, degrees);
	return result;
}
int L_ImageRotateCW(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	ImageRotateCW(image);
	return result;
}
int L_ImageRotateCCW(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	ImageRotateCCW(image);
	return result;
}
int L_ImageColorTint(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	Color color = undefined(S, 1);
	ImageColorTint(image, color);
	return result;
}
int L_ImageColorInvert(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	ImageColorInvert(image);
	return result;
}
int L_ImageColorGrayscale(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	ImageColorGrayscale(image);
	return result;
}
int L_ImageColorContrast(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	float contrast = GetNumber(S, 1);
	ImageColorContrast(image, contrast);
	return result;
}
int L_ImageColorBrightness(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	int brightness = GetInteger(S, 1);
	ImageColorBrightness(image, brightness);
	return result;
}
int L_ImageColorReplace(elf_State *S) {
	int result = 0;
	Image * image = undefined(S, 0);
	Color color = undefined(S, 1);
	Color replace = undefined(S, 2);
	ImageColorReplace(image, color, replace);
	return result;
}
int L_LoadImageColors(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	LoadImageColors(image);
	return result;
}
int L_LoadImagePalette(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	int maxPaletteSize = GetInteger(S, 1);
	int * colorCount = undefined(S, 2);
	LoadImagePalette(image, maxPaletteSize, colorCount);
	return result;
}
int L_UnloadImageColors(elf_State *S) {
	int result = 0;
	Color * colors = undefined(S, 0);
	UnloadImageColors(colors);
	return result;
}
int L_UnloadImagePalette(elf_State *S) {
	int result = 0;
	Color * colors = undefined(S, 0);
	UnloadImagePalette(colors);
	return result;
}
int L_GetImageAlphaBorder(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	float threshold = GetNumber(S, 1);
	GetImageAlphaBorder(image, threshold);
	return result;
}
int L_GetImageColor(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	int x = GetInteger(S, 1);
	int y = GetInteger(S, 2);
	GetImageColor(image, x, y);
	return result;
}
int L_ImageClearBackground(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Color color = undefined(S, 1);
	ImageClearBackground(dst, color);
	return result;
}
int L_ImageDrawPixel(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	int posX = GetInteger(S, 1);
	int posY = GetInteger(S, 2);
	Color color = undefined(S, 3);
	ImageDrawPixel(dst, posX, posY, color);
	return result;
}
int L_ImageDrawPixelV(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Vector2 position = undefined(S, 1);
	Color color = undefined(S, 2);
	ImageDrawPixelV(dst, position, color);
	return result;
}
int L_ImageDrawLine(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	int startPosX = GetInteger(S, 1);
	int startPosY = GetInteger(S, 2);
	int endPosX = GetInteger(S, 3);
	int endPosY = GetInteger(S, 4);
	Color color = undefined(S, 5);
	ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, color);
	return result;
}
int L_ImageDrawLineV(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Vector2 start = undefined(S, 1);
	Vector2 end = undefined(S, 2);
	Color color = undefined(S, 3);
	ImageDrawLineV(dst, start, end, color);
	return result;
}
int L_ImageDrawCircle(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	int centerX = GetInteger(S, 1);
	int centerY = GetInteger(S, 2);
	int radius = GetInteger(S, 3);
	Color color = undefined(S, 4);
	ImageDrawCircle(dst, centerX, centerY, radius, color);
	return result;
}
int L_ImageDrawCircleV(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Vector2 center = undefined(S, 1);
	int radius = GetInteger(S, 2);
	Color color = undefined(S, 3);
	ImageDrawCircleV(dst, center, radius, color);
	return result;
}
int L_ImageDrawCircleLines(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	int centerX = GetInteger(S, 1);
	int centerY = GetInteger(S, 2);
	int radius = GetInteger(S, 3);
	Color color = undefined(S, 4);
	ImageDrawCircleLines(dst, centerX, centerY, radius, color);
	return result;
}
int L_ImageDrawCircleLinesV(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Vector2 center = undefined(S, 1);
	int radius = GetInteger(S, 2);
	Color color = undefined(S, 3);
	ImageDrawCircleLinesV(dst, center, radius, color);
	return result;
}
int L_ImageDrawRectangle(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	int posX = GetInteger(S, 1);
	int posY = GetInteger(S, 2);
	int width = GetInteger(S, 3);
	int height = GetInteger(S, 4);
	Color color = undefined(S, 5);
	ImageDrawRectangle(dst, posX, posY, width, height, color);
	return result;
}
int L_ImageDrawRectangleV(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Vector2 position = undefined(S, 1);
	Vector2 size = undefined(S, 2);
	Color color = undefined(S, 3);
	ImageDrawRectangleV(dst, position, size, color);
	return result;
}
int L_ImageDrawRectangleRec(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Rect rec = undefined(S, 1);
	Color color = undefined(S, 2);
	ImageDrawRectangleRec(dst, rec, color);
	return result;
}
int L_ImageDrawRectangleLines(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Rect rec = undefined(S, 1);
	int thick = GetInteger(S, 2);
	Color color = undefined(S, 3);
	ImageDrawRectangleLines(dst, rec, thick, color);
	return result;
}
int L_ImageDraw(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Image src = undefined(S, 1);
	Rect srcRec = undefined(S, 2);
	Rect dstRec = undefined(S, 3);
	Color tint = undefined(S, 4);
	ImageDraw(dst, src, srcRec, dstRec, tint);
	return result;
}
int L_ImageDrawText(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	const char * text = GetText(S, 1);
	int posX = GetInteger(S, 2);
	int posY = GetInteger(S, 3);
	int fontSize = GetInteger(S, 4);
	Color color = undefined(S, 5);
	ImageDrawText(dst, text, posX, posY, fontSize, color);
	return result;
}
int L_ImageDrawTextEx(elf_State *S) {
	int result = 0;
	Image * dst = undefined(S, 0);
	Font font = undefined(S, 1);
	const char * text = GetText(S, 2);
	Vector2 position = undefined(S, 3);
	float fontSize = GetNumber(S, 4);
	float spacing = GetNumber(S, 5);
	Color tint = undefined(S, 6);
	ImageDrawTextEx(dst, font, text, position, fontSize, spacing, tint);
	return result;
}
int L_LoadTexture(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadTexture(fileName);
	return result;
}
int L_LoadTextureFromImage(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	LoadTextureFromImage(image);
	return result;
}
int L_LoadTextureCubemap(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	int layout = GetInteger(S, 1);
	LoadTextureCubemap(image, layout);
	return result;
}
int L_LoadRenderTexture(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	LoadRenderTexture(width, height);
	return result;
}
int L_IsTextureReady(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	IsTextureReady(texture);
	return result;
}
int L_UnloadTexture(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	UnloadTexture(texture);
	return result;
}
int L_IsRenderTextureReady(elf_State *S) {
	int result = 0;
	RenderTexture2D target = undefined(S, 0);
	IsRenderTextureReady(target);
	return result;
}
int L_UnloadRenderTexture(elf_State *S) {
	int result = 0;
	RenderTexture2D target = undefined(S, 0);
	UnloadRenderTexture(target);
	return result;
}
int L_UpdateTexture(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	const void * pixels = undefined(S, 1);
	UpdateTexture(texture, pixels);
	return result;
}
int L_UpdateTextureRec(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	Rect rec = undefined(S, 1);
	const void * pixels = undefined(S, 2);
	UpdateTextureRec(texture, rec, pixels);
	return result;
}
int L_GenTextureMipmaps(elf_State *S) {
	int result = 0;
	Texture2D * texture = undefined(S, 0);
	GenTextureMipmaps(texture);
	return result;
}
int L_SetTextureFilter(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	int filter = GetInteger(S, 1);
	SetTextureFilter(texture, filter);
	return result;
}
int L_SetTextureWrap(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	int wrap = GetInteger(S, 1);
	SetTextureWrap(texture, wrap);
	return result;
}
int L_DrawTexture(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	int posX = GetInteger(S, 1);
	int posY = GetInteger(S, 2);
	Color tint = undefined(S, 3);
	DrawTexture(texture, posX, posY, tint);
	return result;
}
int L_DrawTextureV(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	Vector2 position = undefined(S, 1);
	Color tint = undefined(S, 2);
	DrawTextureV(texture, position, tint);
	return result;
}
int L_DrawTextureEx(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	Vector2 position = undefined(S, 1);
	float rotation = GetNumber(S, 2);
	float scale = GetNumber(S, 3);
	Color tint = undefined(S, 4);
	DrawTextureEx(texture, position, rotation, scale, tint);
	return result;
}
int L_DrawTextureRec(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	Rect source = undefined(S, 1);
	Vector2 position = undefined(S, 2);
	Color tint = undefined(S, 3);
	DrawTextureRec(texture, source, position, tint);
	return result;
}
int L_DrawTexturePro(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	Rect source = undefined(S, 1);
	Rect dest = undefined(S, 2);
	Vector2 origin = undefined(S, 3);
	float rotation = GetNumber(S, 4);
	Color tint = undefined(S, 5);
	DrawTexturePro(texture, source, dest, origin, rotation, tint);
	return result;
}
int L_DrawTextureNPatch(elf_State *S) {
	int result = 0;
	Texture2D texture = undefined(S, 0);
	NPatchInfo nPatchInfo = undefined(S, 1);
	Rect dest = undefined(S, 2);
	Vector2 origin = undefined(S, 3);
	float rotation = GetNumber(S, 4);
	Color tint = undefined(S, 5);
	DrawTextureNPatch(texture, nPatchInfo, dest, origin, rotation, tint);
	return result;
}
int L_ColorIsEqual(elf_State *S) {
	int result = 0;
	Color col1 = undefined(S, 0);
	Color col2 = undefined(S, 1);
	ColorIsEqual(col1, col2);
	return result;
}
int L_Fade(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	float alpha = GetNumber(S, 1);
	Fade(color, alpha);
	return result;
}
int L_ColorToInt(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	ColorToInt(color);
	return result;
}
int L_ColorNormalize(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	ColorNormalize(color);
	return result;
}
int L_ColorFromNormalized(elf_State *S) {
	int result = 0;
	Vector4 normalized = undefined(S, 0);
	ColorFromNormalized(normalized);
	return result;
}
int L_ColorToHSV(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	ColorToHSV(color);
	return result;
}
int L_ColorFromHSV(elf_State *S) {
	int result = 0;
	float hue = GetNumber(S, 0);
	float saturation = GetNumber(S, 1);
	float value = GetNumber(S, 2);
	ColorFromHSV(hue, saturation, value);
	return result;
}
int L_ColorTint(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	Color tint = undefined(S, 1);
	ColorTint(color, tint);
	return result;
}
int L_ColorBrightness(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	float factor = GetNumber(S, 1);
	ColorBrightness(color, factor);
	return result;
}
int L_ColorContrast(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	float contrast = GetNumber(S, 1);
	ColorContrast(color, contrast);
	return result;
}
int L_ColorAlpha(elf_State *S) {
	int result = 0;
	Color color = undefined(S, 0);
	float alpha = GetNumber(S, 1);
	ColorAlpha(color, alpha);
	return result;
}
int L_ColorAlphaBlend(elf_State *S) {
	int result = 0;
	Color dst = undefined(S, 0);
	Color src = undefined(S, 1);
	Color tint = undefined(S, 2);
	ColorAlphaBlend(dst, src, tint);
	return result;
}
int L_GetColor(elf_State *S) {
	int result = 0;
	unsigned int hexValue = GetInteger(S, 0);
	GetColor(hexValue);
	return result;
}
int L_GetPixelColor(elf_State *S) {
	int result = 0;
	void * srcPtr = undefined(S, 0);
	int format = GetInteger(S, 1);
	GetPixelColor(srcPtr, format);
	return result;
}
int L_SetPixelColor(elf_State *S) {
	int result = 0;
	void * dstPtr = undefined(S, 0);
	Color color = undefined(S, 1);
	int format = GetInteger(S, 2);
	SetPixelColor(dstPtr, color, format);
	return result;
}
int L_GetPixelDataSize(elf_State *S) {
	int result = 0;
	int width = GetInteger(S, 0);
	int height = GetInteger(S, 1);
	int format = GetInteger(S, 2);
	GetPixelDataSize(width, height, format);
	return result;
}
int L_GetFontDefault(elf_State *S) {
	int result = 0;
	GetFontDefault();
	return result;
}
int L_LoadFont(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadFont(fileName);
	return result;
}
int L_LoadFontEx(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	int fontSize = GetInteger(S, 1);
	int * codepoints = undefined(S, 2);
	int codepointCount = GetInteger(S, 3);
	LoadFontEx(fileName, fontSize, codepoints, codepointCount);
	return result;
}
int L_LoadFontFromImage(elf_State *S) {
	int result = 0;
	Image image = undefined(S, 0);
	Color key = undefined(S, 1);
	int firstChar = GetInteger(S, 2);
	LoadFontFromImage(image, key, firstChar);
	return result;
}
int L_LoadFontFromMemory(elf_State *S) {
	int result = 0;
	const char * fileType = GetText(S, 0);
	const unsigned char * fileData = undefined(S, 1);
	int dataSize = GetInteger(S, 2);
	int fontSize = GetInteger(S, 3);
	int * codepoints = undefined(S, 4);
	int codepointCount = GetInteger(S, 5);
	LoadFontFromMemory(fileType, fileData, dataSize, fontSize, codepoints, codepointCount);
	return result;
}
int L_IsFontReady(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	IsFontReady(font);
	return result;
}
int L_LoadFontData(elf_State *S) {
	int result = 0;
	const unsigned char * fileData = undefined(S, 0);
	int dataSize = GetInteger(S, 1);
	int fontSize = GetInteger(S, 2);
	int * codepoints = undefined(S, 3);
	int codepointCount = GetInteger(S, 4);
	int type = GetInteger(S, 5);
	LoadFontData(fileData, dataSize, fontSize, codepoints, codepointCount, type);
	return result;
}
int L_GenImageFontAtlas(elf_State *S) {
	int result = 0;
	const GlyphInfo * glyphs = undefined(S, 0);
	Rect ** glyphRecs = undefined(S, 1);
	int glyphCount = GetInteger(S, 2);
	int fontSize = GetInteger(S, 3);
	int padding = GetInteger(S, 4);
	int packMethod = GetInteger(S, 5);
	GenImageFontAtlas(glyphs, glyphRecs, glyphCount, fontSize, padding, packMethod);
	return result;
}
int L_UnloadFontData(elf_State *S) {
	int result = 0;
	GlyphInfo * glyphs = undefined(S, 0);
	int glyphCount = GetInteger(S, 1);
	UnloadFontData(glyphs, glyphCount);
	return result;
}
int L_UnloadFont(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	UnloadFont(font);
	return result;
}
int L_ExportFontAsCode(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	const char * fileName = GetText(S, 1);
	ExportFontAsCode(font, fileName);
	return result;
}
int L_DrawFPS(elf_State *S) {
	int result = 0;
	int posX = GetInteger(S, 0);
	int posY = GetInteger(S, 1);
	DrawFPS(posX, posY);
	return result;
}
int L_DrawText(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	int posX = GetInteger(S, 1);
	int posY = GetInteger(S, 2);
	int fontSize = GetInteger(S, 3);
	Color color = undefined(S, 4);
	DrawText(text, posX, posY, fontSize, color);
	return result;
}
int L_DrawTextEx(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	const char * text = GetText(S, 1);
	Vector2 position = undefined(S, 2);
	float fontSize = GetNumber(S, 3);
	float spacing = GetNumber(S, 4);
	Color tint = undefined(S, 5);
	DrawTextEx(font, text, position, fontSize, spacing, tint);
	return result;
}
int L_DrawTextPro(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	const char * text = GetText(S, 1);
	Vector2 position = undefined(S, 2);
	Vector2 origin = undefined(S, 3);
	float rotation = GetNumber(S, 4);
	float fontSize = GetNumber(S, 5);
	float spacing = GetNumber(S, 6);
	Color tint = undefined(S, 7);
	DrawTextPro(font, text, position, origin, rotation, fontSize, spacing, tint);
	return result;
}
int L_DrawTextCodepoint(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	int codepoint = GetInteger(S, 1);
	Vector2 position = undefined(S, 2);
	float fontSize = GetNumber(S, 3);
	Color tint = undefined(S, 4);
	DrawTextCodepoint(font, codepoint, position, fontSize, tint);
	return result;
}
int L_DrawTextCodepoints(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	const int * codepoints = undefined(S, 1);
	int codepointCount = GetInteger(S, 2);
	Vector2 position = undefined(S, 3);
	float fontSize = GetNumber(S, 4);
	float spacing = GetNumber(S, 5);
	Color tint = undefined(S, 6);
	DrawTextCodepoints(font, codepoints, codepointCount, position, fontSize, spacing, tint);
	return result;
}
int L_SetTextLineSpacing(elf_State *S) {
	int result = 0;
	int spacing = GetInteger(S, 0);
	SetTextLineSpacing(spacing);
	return result;
}
int L_MeasureText(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	int fontSize = GetInteger(S, 1);
	MeasureText(text, fontSize);
	return result;
}
int L_MeasureTextEx(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	const char * text = GetText(S, 1);
	float fontSize = GetNumber(S, 2);
	float spacing = GetNumber(S, 3);
	MeasureTextEx(font, text, fontSize, spacing);
	return result;
}
int L_GetGlyphIndex(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	int codepoint = GetInteger(S, 1);
	GetGlyphIndex(font, codepoint);
	return result;
}
int L_GetGlyphInfo(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	int codepoint = GetInteger(S, 1);
	GetGlyphInfo(font, codepoint);
	return result;
}
int L_GetGlyphAtlasRec(elf_State *S) {
	int result = 0;
	Font font = undefined(S, 0);
	int codepoint = GetInteger(S, 1);
	GetGlyphAtlasRec(font, codepoint);
	return result;
}
int L_LoadUTF8(elf_State *S) {
	int result = 0;
	const int * codepoints = undefined(S, 0);
	int length = GetInteger(S, 1);
	LoadUTF8(codepoints, length);
	return result;
}
int L_UnloadUTF8(elf_State *S) {
	int result = 0;
	char * text = GetText(S, 0);
	UnloadUTF8(text);
	return result;
}
int L_LoadCodepoints(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	int * count = undefined(S, 1);
	LoadCodepoints(text, count);
	return result;
}
int L_UnloadCodepoints(elf_State *S) {
	int result = 0;
	int * codepoints = undefined(S, 0);
	UnloadCodepoints(codepoints);
	return result;
}
int L_GetCodepointCount(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	GetCodepointCount(text);
	return result;
}
int L_GetCodepoint(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	int * codepointSize = undefined(S, 1);
	GetCodepoint(text, codepointSize);
	return result;
}
int L_GetCodepointNext(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	int * codepointSize = undefined(S, 1);
	GetCodepointNext(text, codepointSize);
	return result;
}
int L_GetCodepointPrevious(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	int * codepointSize = undefined(S, 1);
	GetCodepointPrevious(text, codepointSize);
	return result;
}
int L_CodepointToUTF8(elf_State *S) {
	int result = 0;
	int codepoint = GetInteger(S, 0);
	int * utf8Size = undefined(S, 1);
	CodepointToUTF8(codepoint, utf8Size);
	return result;
}
int L_TextCopy(elf_State *S) {
	int result = 0;
	char * dst = GetText(S, 0);
	const char * src = GetText(S, 1);
	TextCopy(dst, src);
	return result;
}
int L_TextIsEqual(elf_State *S) {
	int result = 0;
	const char * text1 = GetText(S, 0);
	const char * text2 = GetText(S, 1);
	TextIsEqual(text1, text2);
	return result;
}
int L_TextLength(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	TextLength(text);
	return result;
}
int L_TextFormat(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	... args = undefined(S, 1);
	TextFormat(text, args);
	return result;
}
int L_TextSubtext(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	int position = GetInteger(S, 1);
	int length = GetInteger(S, 2);
	TextSubtext(text, position, length);
	return result;
}
int L_TextReplace(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	const char * replace = GetText(S, 1);
	const char * by = GetText(S, 2);
	TextReplace(text, replace, by);
	return result;
}
int L_TextInsert(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	const char * insert = GetText(S, 1);
	int position = GetInteger(S, 2);
	TextInsert(text, insert, position);
	return result;
}
int L_TextJoin(elf_State *S) {
	int result = 0;
	const char ** textList = undefined(S, 0);
	int count = GetInteger(S, 1);
	const char * delimiter = GetText(S, 2);
	TextJoin(textList, count, delimiter);
	return result;
}
int L_TextSplit(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	char delimiter = undefined(S, 1);
	int * count = undefined(S, 2);
	TextSplit(text, delimiter, count);
	return result;
}
int L_TextAppend(elf_State *S) {
	int result = 0;
	char * text = GetText(S, 0);
	const char * append = GetText(S, 1);
	int * position = undefined(S, 2);
	TextAppend(text, append, position);
	return result;
}
int L_TextFindIndex(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	const char * find = GetText(S, 1);
	TextFindIndex(text, find);
	return result;
}
int L_TextToUpper(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	TextToUpper(text);
	return result;
}
int L_TextToLower(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	TextToLower(text);
	return result;
}
int L_TextToPascal(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	TextToPascal(text);
	return result;
}
int L_TextToInteger(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	TextToInteger(text);
	return result;
}
int L_TextToFloat(elf_State *S) {
	int result = 0;
	const char * text = GetText(S, 0);
	TextToFloat(text);
	return result;
}
int L_DrawLine3D(elf_State *S) {
	int result = 0;
	Vector3 startPos = undefined(S, 0);
	Vector3 endPos = undefined(S, 1);
	Color color = undefined(S, 2);
	DrawLine3D(startPos, endPos, color);
	return result;
}
int L_DrawPoint3D(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	Color color = undefined(S, 1);
	DrawPoint3D(position, color);
	return result;
}
int L_DrawCircle3D(elf_State *S) {
	int result = 0;
	Vector3 center = undefined(S, 0);
	float radius = GetNumber(S, 1);
	Vector3 rotationAxis = undefined(S, 2);
	float rotationAngle = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawCircle3D(center, radius, rotationAxis, rotationAngle, color);
	return result;
}
int L_DrawTriangle3D(elf_State *S) {
	int result = 0;
	Vector3 v1 = undefined(S, 0);
	Vector3 v2 = undefined(S, 1);
	Vector3 v3 = undefined(S, 2);
	Color color = undefined(S, 3);
	DrawTriangle3D(v1, v2, v3, color);
	return result;
}
int L_DrawTriangleStrip3D(elf_State *S) {
	int result = 0;
	Vector3 * points = undefined(S, 0);
	int pointCount = GetInteger(S, 1);
	Color color = undefined(S, 2);
	DrawTriangleStrip3D(points, pointCount, color);
	return result;
}
int L_DrawCube(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	float width = GetNumber(S, 1);
	float height = GetNumber(S, 2);
	float length = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawCube(position, width, height, length, color);
	return result;
}
int L_DrawCubeV(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	Vector3 size = undefined(S, 1);
	Color color = undefined(S, 2);
	DrawCubeV(position, size, color);
	return result;
}
int L_DrawCubeWires(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	float width = GetNumber(S, 1);
	float height = GetNumber(S, 2);
	float length = GetNumber(S, 3);
	Color color = undefined(S, 4);
	DrawCubeWires(position, width, height, length, color);
	return result;
}
int L_DrawCubeWiresV(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	Vector3 size = undefined(S, 1);
	Color color = undefined(S, 2);
	DrawCubeWiresV(position, size, color);
	return result;
}
int L_DrawSphere(elf_State *S) {
	int result = 0;
	Vector3 centerPos = undefined(S, 0);
	float radius = GetNumber(S, 1);
	Color color = undefined(S, 2);
	DrawSphere(centerPos, radius, color);
	return result;
}
int L_DrawSphereEx(elf_State *S) {
	int result = 0;
	Vector3 centerPos = undefined(S, 0);
	float radius = GetNumber(S, 1);
	int rings = GetInteger(S, 2);
	int slices = GetInteger(S, 3);
	Color color = undefined(S, 4);
	DrawSphereEx(centerPos, radius, rings, slices, color);
	return result;
}
int L_DrawSphereWires(elf_State *S) {
	int result = 0;
	Vector3 centerPos = undefined(S, 0);
	float radius = GetNumber(S, 1);
	int rings = GetInteger(S, 2);
	int slices = GetInteger(S, 3);
	Color color = undefined(S, 4);
	DrawSphereWires(centerPos, radius, rings, slices, color);
	return result;
}
int L_DrawCylinder(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	float radiusTop = GetNumber(S, 1);
	float radiusBottom = GetNumber(S, 2);
	float height = GetNumber(S, 3);
	int slices = GetInteger(S, 4);
	Color color = undefined(S, 5);
	DrawCylinder(position, radiusTop, radiusBottom, height, slices, color);
	return result;
}
int L_DrawCylinderEx(elf_State *S) {
	int result = 0;
	Vector3 startPos = undefined(S, 0);
	Vector3 endPos = undefined(S, 1);
	float startRadius = GetNumber(S, 2);
	float endRadius = GetNumber(S, 3);
	int sides = GetInteger(S, 4);
	Color color = undefined(S, 5);
	DrawCylinderEx(startPos, endPos, startRadius, endRadius, sides, color);
	return result;
}
int L_DrawCylinderWires(elf_State *S) {
	int result = 0;
	Vector3 position = undefined(S, 0);
	float radiusTop = GetNumber(S, 1);
	float radiusBottom = GetNumber(S, 2);
	float height = GetNumber(S, 3);
	int slices = GetInteger(S, 4);
	Color color = undefined(S, 5);
	DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color);
	return result;
}
int L_DrawCylinderWiresEx(elf_State *S) {
	int result = 0;
	Vector3 startPos = undefined(S, 0);
	Vector3 endPos = undefined(S, 1);
	float startRadius = GetNumber(S, 2);
	float endRadius = GetNumber(S, 3);
	int sides = GetInteger(S, 4);
	Color color = undefined(S, 5);
	DrawCylinderWiresEx(startPos, endPos, startRadius, endRadius, sides, color);
	return result;
}
int L_DrawCapsule(elf_State *S) {
	int result = 0;
	Vector3 startPos = undefined(S, 0);
	Vector3 endPos = undefined(S, 1);
	float radius = GetNumber(S, 2);
	int slices = GetInteger(S, 3);
	int rings = GetInteger(S, 4);
	Color color = undefined(S, 5);
	DrawCapsule(startPos, endPos, radius, slices, rings, color);
	return result;
}
int L_DrawCapsuleWires(elf_State *S) {
	int result = 0;
	Vector3 startPos = undefined(S, 0);
	Vector3 endPos = undefined(S, 1);
	float radius = GetNumber(S, 2);
	int slices = GetInteger(S, 3);
	int rings = GetInteger(S, 4);
	Color color = undefined(S, 5);
	DrawCapsuleWires(startPos, endPos, radius, slices, rings, color);
	return result;
}
int L_DrawPlane(elf_State *S) {
	int result = 0;
	Vector3 centerPos = undefined(S, 0);
	Vector2 size = undefined(S, 1);
	Color color = undefined(S, 2);
	DrawPlane(centerPos, size, color);
	return result;
}
int L_DrawRay(elf_State *S) {
	int result = 0;
	Ray ray = undefined(S, 0);
	Color color = undefined(S, 1);
	DrawRay(ray, color);
	return result;
}
int L_DrawGrid(elf_State *S) {
	int result = 0;
	int slices = GetInteger(S, 0);
	float spacing = GetNumber(S, 1);
	DrawGrid(slices, spacing);
	return result;
}
int L_LoadModel(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadModel(fileName);
	return result;
}
int L_LoadModelFromMesh(elf_State *S) {
	int result = 0;
	Mesh mesh = undefined(S, 0);
	LoadModelFromMesh(mesh);
	return result;
}
int L_IsModelReady(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	IsModelReady(model);
	return result;
}
int L_UnloadModel(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	UnloadModel(model);
	return result;
}
int L_GetModelBoundingBox(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	GetModelBoundingBox(model);
	return result;
}
int L_DrawModel(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	Vector3 position = undefined(S, 1);
	float scale = GetNumber(S, 2);
	Color tint = undefined(S, 3);
	DrawModel(model, position, scale, tint);
	return result;
}
int L_DrawModelEx(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	Vector3 position = undefined(S, 1);
	Vector3 rotationAxis = undefined(S, 2);
	float rotationAngle = GetNumber(S, 3);
	Vector3 scale = undefined(S, 4);
	Color tint = undefined(S, 5);
	DrawModelEx(model, position, rotationAxis, rotationAngle, scale, tint);
	return result;
}
int L_DrawModelWires(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	Vector3 position = undefined(S, 1);
	float scale = GetNumber(S, 2);
	Color tint = undefined(S, 3);
	DrawModelWires(model, position, scale, tint);
	return result;
}
int L_DrawModelWiresEx(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	Vector3 position = undefined(S, 1);
	Vector3 rotationAxis = undefined(S, 2);
	float rotationAngle = GetNumber(S, 3);
	Vector3 scale = undefined(S, 4);
	Color tint = undefined(S, 5);
	DrawModelWiresEx(model, position, rotationAxis, rotationAngle, scale, tint);
	return result;
}
int L_DrawBoundingBox(elf_State *S) {
	int result = 0;
	BoundingBox box = undefined(S, 0);
	Color color = undefined(S, 1);
	DrawBoundingBox(box, color);
	return result;
}
int L_DrawBillboard(elf_State *S) {
	int result = 0;
	Camera camera = undefined(S, 0);
	Texture2D texture = undefined(S, 1);
	Vector3 position = undefined(S, 2);
	float size = GetNumber(S, 3);
	Color tint = undefined(S, 4);
	DrawBillboard(camera, texture, position, size, tint);
	return result;
}
int L_DrawBillboardRec(elf_State *S) {
	int result = 0;
	Camera camera = undefined(S, 0);
	Texture2D texture = undefined(S, 1);
	Rect source = undefined(S, 2);
	Vector3 position = undefined(S, 3);
	Vector2 size = undefined(S, 4);
	Color tint = undefined(S, 5);
	DrawBillboardRec(camera, texture, source, position, size, tint);
	return result;
}
int L_DrawBillboardPro(elf_State *S) {
	int result = 0;
	Camera camera = undefined(S, 0);
	Texture2D texture = undefined(S, 1);
	Rect source = undefined(S, 2);
	Vector3 position = undefined(S, 3);
	Vector3 up = undefined(S, 4);
	Vector2 size = undefined(S, 5);
	Vector2 origin = undefined(S, 6);
	float rotation = GetNumber(S, 7);
	Color tint = undefined(S, 8);
	DrawBillboardPro(camera, texture, source, position, up, size, origin, rotation, tint);
	return result;
}
int L_UploadMesh(elf_State *S) {
	int result = 0;
	Mesh * mesh = undefined(S, 0);
	bool dynamic = GetInteger(S, 1);
	UploadMesh(mesh, dynamic);
	return result;
}
int L_UpdateMeshBuffer(elf_State *S) {
	int result = 0;
	Mesh mesh = undefined(S, 0);
	int index = GetInteger(S, 1);
	const void * data = undefined(S, 2);
	int dataSize = GetInteger(S, 3);
	int offset = GetInteger(S, 4);
	UpdateMeshBuffer(mesh, index, data, dataSize, offset);
	return result;
}
int L_UnloadMesh(elf_State *S) {
	int result = 0;
	Mesh mesh = undefined(S, 0);
	UnloadMesh(mesh);
	return result;
}
int L_DrawMesh(elf_State *S) {
	int result = 0;
	Mesh mesh = undefined(S, 0);
	Material material = undefined(S, 1);
	Matrix transform = undefined(S, 2);
	DrawMesh(mesh, material, transform);
	return result;
}
int L_DrawMeshInstanced(elf_State *S) {
	int result = 0;
	Mesh mesh = undefined(S, 0);
	Material material = undefined(S, 1);
	const Matrix * transforms = undefined(S, 2);
	int instances = GetInteger(S, 3);
	DrawMeshInstanced(mesh, material, transforms, instances);
	return result;
}
int L_GetMeshBoundingBox(elf_State *S) {
	int result = 0;
	Mesh mesh = undefined(S, 0);
	GetMeshBoundingBox(mesh);
	return result;
}
int L_GenMeshTangents(elf_State *S) {
	int result = 0;
	Mesh * mesh = undefined(S, 0);
	GenMeshTangents(mesh);
	return result;
}
int L_ExportMesh(elf_State *S) {
	int result = 0;
	Mesh mesh = undefined(S, 0);
	const char * fileName = GetText(S, 1);
	ExportMesh(mesh, fileName);
	return result;
}
int L_ExportMeshAsCode(elf_State *S) {
	int result = 0;
	Mesh mesh = undefined(S, 0);
	const char * fileName = GetText(S, 1);
	ExportMeshAsCode(mesh, fileName);
	return result;
}
int L_GenMeshPoly(elf_State *S) {
	int result = 0;
	int sides = GetInteger(S, 0);
	float radius = GetNumber(S, 1);
	GenMeshPoly(sides, radius);
	return result;
}
int L_GenMeshPlane(elf_State *S) {
	int result = 0;
	float width = GetNumber(S, 0);
	float length = GetNumber(S, 1);
	int resX = GetInteger(S, 2);
	int resZ = GetInteger(S, 3);
	GenMeshPlane(width, length, resX, resZ);
	return result;
}
int L_GenMeshCube(elf_State *S) {
	int result = 0;
	float width = GetNumber(S, 0);
	float height = GetNumber(S, 1);
	float length = GetNumber(S, 2);
	GenMeshCube(width, height, length);
	return result;
}
int L_GenMeshSphere(elf_State *S) {
	int result = 0;
	float radius = GetNumber(S, 0);
	int rings = GetInteger(S, 1);
	int slices = GetInteger(S, 2);
	GenMeshSphere(radius, rings, slices);
	return result;
}
int L_GenMeshHemiSphere(elf_State *S) {
	int result = 0;
	float radius = GetNumber(S, 0);
	int rings = GetInteger(S, 1);
	int slices = GetInteger(S, 2);
	GenMeshHemiSphere(radius, rings, slices);
	return result;
}
int L_GenMeshCylinder(elf_State *S) {
	int result = 0;
	float radius = GetNumber(S, 0);
	float height = GetNumber(S, 1);
	int slices = GetInteger(S, 2);
	GenMeshCylinder(radius, height, slices);
	return result;
}
int L_GenMeshCone(elf_State *S) {
	int result = 0;
	float radius = GetNumber(S, 0);
	float height = GetNumber(S, 1);
	int slices = GetInteger(S, 2);
	GenMeshCone(radius, height, slices);
	return result;
}
int L_GenMeshTorus(elf_State *S) {
	int result = 0;
	float radius = GetNumber(S, 0);
	float size = GetNumber(S, 1);
	int radSeg = GetInteger(S, 2);
	int sides = GetInteger(S, 3);
	GenMeshTorus(radius, size, radSeg, sides);
	return result;
}
int L_GenMeshKnot(elf_State *S) {
	int result = 0;
	float radius = GetNumber(S, 0);
	float size = GetNumber(S, 1);
	int radSeg = GetInteger(S, 2);
	int sides = GetInteger(S, 3);
	GenMeshKnot(radius, size, radSeg, sides);
	return result;
}
int L_GenMeshHeightmap(elf_State *S) {
	int result = 0;
	Image heightmap = undefined(S, 0);
	Vector3 size = undefined(S, 1);
	GenMeshHeightmap(heightmap, size);
	return result;
}
int L_GenMeshCubicmap(elf_State *S) {
	int result = 0;
	Image cubicmap = undefined(S, 0);
	Vector3 cubeSize = undefined(S, 1);
	GenMeshCubicmap(cubicmap, cubeSize);
	return result;
}
int L_LoadMaterials(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	int * materialCount = undefined(S, 1);
	LoadMaterials(fileName, materialCount);
	return result;
}
int L_LoadMaterialDefault(elf_State *S) {
	int result = 0;
	LoadMaterialDefault();
	return result;
}
int L_IsMaterialReady(elf_State *S) {
	int result = 0;
	Material material = undefined(S, 0);
	IsMaterialReady(material);
	return result;
}
int L_UnloadMaterial(elf_State *S) {
	int result = 0;
	Material material = undefined(S, 0);
	UnloadMaterial(material);
	return result;
}
int L_SetMaterialTexture(elf_State *S) {
	int result = 0;
	Material * material = undefined(S, 0);
	int mapType = GetInteger(S, 1);
	Texture2D texture = undefined(S, 2);
	SetMaterialTexture(material, mapType, texture);
	return result;
}
int L_SetModelMeshMaterial(elf_State *S) {
	int result = 0;
	Model * model = undefined(S, 0);
	int meshId = GetInteger(S, 1);
	int materialId = GetInteger(S, 2);
	SetModelMeshMaterial(model, meshId, materialId);
	return result;
}
int L_LoadModelAnimations(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	int * animCount = undefined(S, 1);
	LoadModelAnimations(fileName, animCount);
	return result;
}
int L_UpdateModelAnimation(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	ModelAnimation anim = undefined(S, 1);
	int frame = GetInteger(S, 2);
	UpdateModelAnimation(model, anim, frame);
	return result;
}
int L_UnloadModelAnimation(elf_State *S) {
	int result = 0;
	ModelAnimation anim = undefined(S, 0);
	UnloadModelAnimation(anim);
	return result;
}
int L_UnloadModelAnimations(elf_State *S) {
	int result = 0;
	ModelAnimation * animations = undefined(S, 0);
	int animCount = GetInteger(S, 1);
	UnloadModelAnimations(animations, animCount);
	return result;
}
int L_IsModelAnimationValid(elf_State *S) {
	int result = 0;
	Model model = undefined(S, 0);
	ModelAnimation anim = undefined(S, 1);
	IsModelAnimationValid(model, anim);
	return result;
}
int L_CheckCollisionSpheres(elf_State *S) {
	int result = 0;
	Vector3 center1 = undefined(S, 0);
	float radius1 = GetNumber(S, 1);
	Vector3 center2 = undefined(S, 2);
	float radius2 = GetNumber(S, 3);
	CheckCollisionSpheres(center1, radius1, center2, radius2);
	return result;
}
int L_CheckCollisionBoxes(elf_State *S) {
	int result = 0;
	BoundingBox box1 = undefined(S, 0);
	BoundingBox box2 = undefined(S, 1);
	CheckCollisionBoxes(box1, box2);
	return result;
}
int L_CheckCollisionBoxSphere(elf_State *S) {
	int result = 0;
	BoundingBox box = undefined(S, 0);
	Vector3 center = undefined(S, 1);
	float radius = GetNumber(S, 2);
	CheckCollisionBoxSphere(box, center, radius);
	return result;
}
int L_GetRayCollisionSphere(elf_State *S) {
	int result = 0;
	Ray ray = undefined(S, 0);
	Vector3 center = undefined(S, 1);
	float radius = GetNumber(S, 2);
	GetRayCollisionSphere(ray, center, radius);
	return result;
}
int L_GetRayCollisionBox(elf_State *S) {
	int result = 0;
	Ray ray = undefined(S, 0);
	BoundingBox box = undefined(S, 1);
	GetRayCollisionBox(ray, box);
	return result;
}
int L_GetRayCollisionMesh(elf_State *S) {
	int result = 0;
	Ray ray = undefined(S, 0);
	Mesh mesh = undefined(S, 1);
	Matrix transform = undefined(S, 2);
	GetRayCollisionMesh(ray, mesh, transform);
	return result;
}
int L_GetRayCollisionTriangle(elf_State *S) {
	int result = 0;
	Ray ray = undefined(S, 0);
	Vector3 p1 = undefined(S, 1);
	Vector3 p2 = undefined(S, 2);
	Vector3 p3 = undefined(S, 3);
	GetRayCollisionTriangle(ray, p1, p2, p3);
	return result;
}
int L_GetRayCollisionQuad(elf_State *S) {
	int result = 0;
	Ray ray = undefined(S, 0);
	Vector3 p1 = undefined(S, 1);
	Vector3 p2 = undefined(S, 2);
	Vector3 p3 = undefined(S, 3);
	Vector3 p4 = undefined(S, 4);
	GetRayCollisionQuad(ray, p1, p2, p3, p4);
	return result;
}
int L_InitAudioDevice(elf_State *S) {
	int result = 0;
	InitAudioDevice();
	return result;
}
int L_CloseAudioDevice(elf_State *S) {
	int result = 0;
	CloseAudioDevice();
	return result;
}
int L_IsAudioDeviceReady(elf_State *S) {
	int result = 0;
	IsAudioDeviceReady();
	return result;
}
int L_SetMasterVolume(elf_State *S) {
	int result = 0;
	float volume = GetNumber(S, 0);
	SetMasterVolume(volume);
	return result;
}
int L_GetMasterVolume(elf_State *S) {
	int result = 0;
	GetMasterVolume();
	return result;
}
int L_LoadWave(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadWave(fileName);
	return result;
}
int L_LoadWaveFromMemory(elf_State *S) {
	int result = 0;
	const char * fileType = GetText(S, 0);
	const unsigned char * fileData = undefined(S, 1);
	int dataSize = GetInteger(S, 2);
	LoadWaveFromMemory(fileType, fileData, dataSize);
	return result;
}
int L_IsWaveReady(elf_State *S) {
	int result = 0;
	Wave wave = undefined(S, 0);
	IsWaveReady(wave);
	return result;
}
int L_LoadSound(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadSound(fileName);
	return result;
}
int L_LoadSoundFromWave(elf_State *S) {
	int result = 0;
	Wave wave = undefined(S, 0);
	LoadSoundFromWave(wave);
	return result;
}
int L_LoadSoundAlias(elf_State *S) {
	int result = 0;
	Sound source = undefined(S, 0);
	LoadSoundAlias(source);
	return result;
}
int L_IsSoundReady(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	IsSoundReady(sound);
	return result;
}
int L_UpdateSound(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	const void * data = undefined(S, 1);
	int sampleCount = GetInteger(S, 2);
	UpdateSound(sound, data, sampleCount);
	return result;
}
int L_UnloadWave(elf_State *S) {
	int result = 0;
	Wave wave = undefined(S, 0);
	UnloadWave(wave);
	return result;
}
int L_UnloadSound(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	UnloadSound(sound);
	return result;
}
int L_UnloadSoundAlias(elf_State *S) {
	int result = 0;
	Sound alias = undefined(S, 0);
	UnloadSoundAlias(alias);
	return result;
}
int L_ExportWave(elf_State *S) {
	int result = 0;
	Wave wave = undefined(S, 0);
	const char * fileName = GetText(S, 1);
	ExportWave(wave, fileName);
	return result;
}
int L_ExportWaveAsCode(elf_State *S) {
	int result = 0;
	Wave wave = undefined(S, 0);
	const char * fileName = GetText(S, 1);
	ExportWaveAsCode(wave, fileName);
	return result;
}
int L_PlaySound(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	A_PlaySound(sound);
	return result;
}
int L_StopSound(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	StopSound(sound);
	return result;
}
int L_PauseSound(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	PauseSound(sound);
	return result;
}
int L_ResumeSound(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	ResumeSound(sound);
	return result;
}
int L_IsSoundPlaying(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	IsSoundPlaying(sound);
	return result;
}
int L_SetSoundVolume(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	float volume = GetNumber(S, 1);
	SetSoundVolume(sound, volume);
	return result;
}
int L_SetSoundPitch(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	float pitch = GetNumber(S, 1);
	SetSoundPitch(sound, pitch);
	return result;
}
int L_SetSoundPan(elf_State *S) {
	int result = 0;
	Sound sound = undefined(S, 0);
	float pan = GetNumber(S, 1);
	SetSoundPan(sound, pan);
	return result;
}
int L_WaveCopy(elf_State *S) {
	int result = 0;
	Wave wave = undefined(S, 0);
	WaveCopy(wave);
	return result;
}
int L_WaveCrop(elf_State *S) {
	int result = 0;
	Wave * wave = undefined(S, 0);
	int initSample = GetInteger(S, 1);
	int finalSample = GetInteger(S, 2);
	WaveCrop(wave, initSample, finalSample);
	return result;
}
int L_WaveFormat(elf_State *S) {
	int result = 0;
	Wave * wave = undefined(S, 0);
	int sampleRate = GetInteger(S, 1);
	int sampleSize = GetInteger(S, 2);
	int channels = GetInteger(S, 3);
	WaveFormat(wave, sampleRate, sampleSize, channels);
	return result;
}
int L_LoadWaveSamples(elf_State *S) {
	int result = 0;
	Wave wave = undefined(S, 0);
	LoadWaveSamples(wave);
	return result;
}
int L_UnloadWaveSamples(elf_State *S) {
	int result = 0;
	float * samples = undefined(S, 0);
	UnloadWaveSamples(samples);
	return result;
}
int L_LoadMusicStream(elf_State *S) {
	int result = 0;
	const char * fileName = GetText(S, 0);
	LoadMusicStream(fileName);
	return result;
}
int L_LoadMusicStreamFromMemory(elf_State *S) {
	int result = 0;
	const char * fileType = GetText(S, 0);
	const unsigned char * data = undefined(S, 1);
	int dataSize = GetInteger(S, 2);
	LoadMusicStreamFromMemory(fileType, data, dataSize);
	return result;
}
int L_IsMusicReady(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	IsMusicReady(music);
	return result;
}
int L_UnloadMusicStream(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	UnloadMusicStream(music);
	return result;
}
int L_PlayMusicStream(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	PlayMusicStream(music);
	return result;
}
int L_IsMusicStreamPlaying(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	IsMusicStreamPlaying(music);
	return result;
}
int L_UpdateMusicStream(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	UpdateMusicStream(music);
	return result;
}
int L_StopMusicStream(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	StopMusicStream(music);
	return result;
}
int L_PauseMusicStream(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	PauseMusicStream(music);
	return result;
}
int L_ResumeMusicStream(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	ResumeMusicStream(music);
	return result;
}
int L_SeekMusicStream(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	float position = GetNumber(S, 1);
	SeekMusicStream(music, position);
	return result;
}
int L_SetMusicVolume(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	float volume = GetNumber(S, 1);
	SetMusicVolume(music, volume);
	return result;
}
int L_SetMusicPitch(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	float pitch = GetNumber(S, 1);
	SetMusicPitch(music, pitch);
	return result;
}
int L_SetMusicPan(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	float pan = GetNumber(S, 1);
	SetMusicPan(music, pan);
	return result;
}
int L_GetMusicTimeLength(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	GetMusicTimeLength(music);
	return result;
}
int L_GetMusicTimePlayed(elf_State *S) {
	int result = 0;
	Music music = undefined(S, 0);
	GetMusicTimePlayed(music);
	return result;
}
int L_LoadAudioStream(elf_State *S) {
	int result = 0;
	unsigned int sampleRate = GetInteger(S, 0);
	unsigned int sampleSize = GetInteger(S, 1);
	unsigned int channels = GetInteger(S, 2);
	LoadAudioStream(sampleRate, sampleSize, channels);
	return result;
}
int L_IsAudioStreamReady(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	IsAudioStreamReady(stream);
	return result;
}
int L_UnloadAudioStream(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	UnloadAudioStream(stream);
	return result;
}
int L_UpdateAudioStream(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	const void * data = undefined(S, 1);
	int frameCount = GetInteger(S, 2);
	UpdateAudioStream(stream, data, frameCount);
	return result;
}
int L_IsAudioStreamProcessed(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	IsAudioStreamProcessed(stream);
	return result;
}
int L_PlayAudioStream(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	PlayAudioStream(stream);
	return result;
}
int L_PauseAudioStream(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	PauseAudioStream(stream);
	return result;
}
int L_ResumeAudioStream(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	ResumeAudioStream(stream);
	return result;
}
int L_IsAudioStreamPlaying(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	IsAudioStreamPlaying(stream);
	return result;
}
int L_StopAudioStream(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	StopAudioStream(stream);
	return result;
}
int L_SetAudioStreamVolume(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	float volume = GetNumber(S, 1);
	SetAudioStreamVolume(stream, volume);
	return result;
}
int L_SetAudioStreamPitch(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	float pitch = GetNumber(S, 1);
	SetAudioStreamPitch(stream, pitch);
	return result;
}
int L_SetAudioStreamPan(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	float pan = GetNumber(S, 1);
	SetAudioStreamPan(stream, pan);
	return result;
}
int L_SetAudioStreamBufferSizeDefault(elf_State *S) {
	int result = 0;
	int size = GetInteger(S, 0);
	SetAudioStreamBufferSizeDefault(size);
	return result;
}
int L_SetAudioStreamCallback(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	AudioCallback callback = undefined(S, 1);
	SetAudioStreamCallback(stream, callback);
	return result;
}
int L_AttachAudioStreamProcessor(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	AudioCallback processor = undefined(S, 1);
	AttachAudioStreamProcessor(stream, processor);
	return result;
}
int L_DetachAudioStreamProcessor(elf_State *S) {
	int result = 0;
	AudioStream stream = undefined(S, 0);
	AudioCallback processor = undefined(S, 1);
	DetachAudioStreamProcessor(stream, processor);
	return result;
}
int L_AttachAudioMixedProcessor(elf_State *S) {
	int result = 0;
	AudioCallback processor = undefined(S, 0);
	AttachAudioMixedProcessor(processor);
	return result;
}
int L_DetachAudioMixedProcessor(elf_State *S) {
	int result = 0;
	AudioCallback processor = undefined(S, 0);
	DetachAudioMixedProcessor(processor);
	return result;
}
