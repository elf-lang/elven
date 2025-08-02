#if !defined(_DEBUG)
#error "error"
#endif
// <3
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "elements.h"
#include "platform.h"
#include "draw_2d.h"
#include "renderer.h"


enum {
	FONTS_CAPACITY       = 16,
	FONT_NAME_CAPACITY   = 128,
	FONT_GLYPHS_CAPACITY = 256,
};

typedef struct D_FONT D_FONT;
struct D_FONT {
	TextureId       texture;
	char              name[FONT_NAME_CAPACITY];
	stbtt_bakedchar glyphs[FONT_GLYPHS_CAPACITY];
};

global D_FONT g_fonts[FONTS_CAPACITY];

int InstallFont(R_Renderer *rend, int id, char *name, int size) {
	// todo:
	D_FONT *font = &g_fonts[id];

	// todo:
	enum { TEMP_SIZE = MEGABYTES(8) };
	unsigned char *data = malloc(TEMP_SIZE);
	OS_ReadEntireFile(name, data, TEMP_SIZE);

	int r = 1024;

	unsigned char *atlas = malloc(r * r);
	int error = stbtt_BakeFontBitmap(data, 0, size, atlas, r, r, 32, 95, (stbtt_bakedchar *) font->glyphs);
	if (error == 0) {
		fprintf(stderr, "Could Not Create Font\n");
	}

	font->texture = R_FindFreeTexture(rend);
	R_InstallTexture(rend, font->texture, FORMAT_R8_UNORM, (vec2i){r, r}, atlas);

	free(data);
	free(atlas);

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

	return id;
}




//
// todo: proper text rendering...
//
void D_DrawText(R_Renderer *rend, f32 x, f32 y, char *text) {
	// why would you even call
	ASSERT(text);

	Color color = D_GetColor0();

	D_FONT *font = & g_fonts[D_GetFont()];

	D_SetTexture(rend, font->texture);
	D_BeginQuads(rend);

	while (*text) {
		i32 token = *text ++;
		i32 index = token - 32;
		stbtt_bakedchar glyph = font->glyphs[index];
		if (token != ' ') {
			iRect src_r = { glyph.x0, glyph.y0, glyph.x1 - glyph.x0, glyph.y1 - glyph.y0 };
			Rect dst_r = { x + glyph.xoff, y - (src_r.h + glyph.yoff), src_r.w, src_r.h };
			D_PushQuad(rend, dst_r, src_r);
		}
		x += glyph.xadvance * D_GetScale().x;
	}

	D_EndQuads(rend);
}