#if !defined(_DEBUG)
#error "error"
#endif


// <3
#define STBTT_STATIC
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

// todo: this whole thing
int InstallFont(R_Renderer *rend, int id, char *name, int size) {
	D_FONT *font = &g_fonts[id];

	enum { TEMP_SIZE = MEGABYTES(8) };
	unsigned char *data = malloc(TEMP_SIZE);
	OS_ReadEntireFile(name, data, TEMP_SIZE);

	// todo:
	int num_chars = 95;
	int atlas_size = sqrt((size + 8) * (size + 8) * num_chars);

	unsigned char *atlas = malloc(atlas_size * atlas_size);

	int error = stbtt_BakeFontBitmap(data, 0, size, atlas
	, atlas_size, atlas_size, 32, 95, (stbtt_bakedchar *) font->glyphs);

	if (error == 0) {
		fprintf(stderr, "Could Not Create Font\n");
	}

	font->texture = R_FindFreeTexture(rend);
	R_InstallTexture(rend, font->texture, FORMAT_R8_UNORM, (vec2i){atlas_size, atlas_size}, atlas);

	free(data);
	free(atlas);
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

	TextureId texture_prev = R_GetTexture(rend);

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

	D_SetTexture(rend, texture_prev);
}