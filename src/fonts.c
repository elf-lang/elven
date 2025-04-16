
static i32 LoadFileContents(char *name, void *memory, i32 max_bytes_to_read) {

	HANDLE file = INVALID_HANDLE_VALUE;
	do {
		file = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
		if (file == INVALID_HANDLE_VALUE) {
			DWORD error = GetLastError();
			if (error == ERROR_SHARING_VIOLATION) {
				Sleep(100);
			} else {
				os_error_dialog(0);
				return 0;
			}
		}
	} while(file == INVALID_HANDLE_VALUE);

	DWORD num_bytes_to_read = GetFileSize(file, NULL);
	num_bytes_to_read = MIN(num_bytes_to_read, max_bytes_to_read);

	DWORD bytes_read = 0;
	ReadFile(file, memory, num_bytes_to_read, &bytes_read, NULL);
	ASSERT(bytes_read == num_bytes_to_read);

	CloseHandle(file);

	return bytes_read;
}

static JFont *LoadFont(jam_State *J, char *name) {

	// todo:
	JFont *font = calloc(1, sizeof(*font));

	enum { TEMP_SIZE = MEGABYTES(8) };

	unsigned char *data = malloc(TEMP_SIZE);
	LoadFileContents(name, data, TEMP_SIZE);

	int h = 32;
	int r = 1024;

	unsigned char *atlas = malloc(r * r);
	int error = stbtt_BakeFontBitmap(data, 0, h, atlas, r, r, 32, 95, (stbtt_bakedchar *) font->glyphs);
	if (error == 0) {
		fprintf(stderr, "Could Not Create Font\n");
	}

	font->texture = TEXTURE_FONT;
	rInstallTexture(J, TEXTURE_FONT, FORMAT_R_U8, (vec2i){r, r}, atlas);

	free(data);

	return font;
	// char buffer[256];
	// {
	// 	sprintf_s(buffer, sizeof(buffer), "%s.font", output_name, buffer);
	// 	FILE *file = fopen(buffer, "wb");
	// 	if (!file) fprintf(stderr, "invalid name");
	// 	fwrite(&ff, 1, sizeof(ff), file);
	// 	fclose(file);
	// }
	// {
	// 	sprintf_s(buffer, sizeof(buffer), "%s.bmp", output_name, buffer);
	// 	stbi_write_bmp(buffer,r,r,1,atlas);
	// }
}