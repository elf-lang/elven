

static D_FONT *LoadFont(jam_State *J, char *name) {

	// todo:
	D_FONT *font = calloc(1, sizeof(*font));

	enum { TEMP_SIZE = MEGABYTES(8) };

	unsigned char *data = malloc(TEMP_SIZE);
	OS_ReadEntireFile(name, data, TEMP_SIZE);

	int h = 32;
	int r = 1024;

	unsigned char *atlas = malloc(r * r);
	int error = stbtt_BakeFontBitmap(data, 0, h, atlas, r, r, 32, 95, (stbtt_bakedchar *) font->glyphs);
	if (error == 0) {
		fprintf(stderr, "Could Not Create Font\n");
	}

	// font->texture = TEXTURE_FONT;
	// R_InstallTexture(J, TEXTURE_FONT, FORMAT_R_U8, (vec2i){r, r}, atlas);

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