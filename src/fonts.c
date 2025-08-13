#if !defined(_DEBUG)
#error "error"
#endif


#include "bff.h"

#if 0
// <3
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#endif

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "elements.h"
#include "platform.h"
#include "draw_2d.h"
#include "renderer.h"
#include "include\elf.h"
#include "src\string_builder.c"
#include "src\path_builder.c"
#include "src\platform\system.h"

enum {
	FONTS_CAPACITY       = 16,
	FONT_NAME_CAPACITY   = 128,
	FONT_GLYPHS_CAPACITY = 256,
};



typedef struct D_FONT D_FONT;
struct D_FONT {
	TextureId   texture;
	char        name[FONT_NAME_CAPACITY];
	bff_Glyph   glyphs[FONT_GLYPHS_CAPACITY];
};



global D_FONT g_fonts[FONTS_CAPACITY];




// todo: this whole thing
int
InstallFont(R_Renderer *rend, int id, char *name, int font_size) {
	D_FONT *font = &g_fonts[id];

	elf_Handle file = sys_open_file(name, SYS_OPEN_READ, SYS_OPEN_ALWAYS);
	int size = sys_size_file(file);
	bff_FILE_DATA *data = malloc(size);
	sys_read_file(file, data, size);
	sys_close_file(file);

	memcpy(font->glyphs, data->glyphs, sizeof(data->glyphs));

	vec2i atlas_reso = { data->atlas_width, data->atlas_height };

	font->texture = TEXTURE_CAPACITY - 1 - id;
	R_InstallTexture(rend, font->texture, FORMAT_R8_UNORM, atlas_reso, data->atlas_data);



#if 0
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

	for (int i = 0; i < atlas_size * atlas_size; i ++) {
		atlas[i] = atlas[i] > 0 ? 255 : 0;
	}

	if (error == 0) {
		fprintf(stderr, "Could Not Create Font\n");
	}

	// todo: this is weird!
	font->texture = TEXTURE_CAPACITY - 1 - id;
	R_InstallTexture(rend, font->texture, FORMAT_R8_UNORM, (vec2i){atlas_size, atlas_size}, atlas);

	free(data);
	free(atlas);
#endif
	return id;
}


f32 D_MeasureText(const char *text) {
	f32 width = 0.0;

	D_FONT *font = & g_fonts[D_GetFont()];
	while (*text) {
		i32 token = *text ++;
		i32 index = token - 32;

		bff_Glyph glyph = font->glyphs[index];
		width += glyph.x_advance;
	}

	return width * D_GetScale().x;
}

//
// todo: proper text rendering...
//
void D_DrawText(R_Renderer *rend, f32 x, f32 y, char *text) {
	// why would you even call
	ASSERT(text);

	D_FONT *font = & g_fonts[D_GetFont()];

	Color color = D_GetColor0();


	TextureId texture_prev = R_GetTexture(rend);

	D_SetTexture(rend, font->texture);
	D_BeginQuads(rend);

	while (*text) {
		i32 token = *text ++;
		// i32 index = token - 32;
		i32 index = token;
		bff_Glyph glyph = font->glyphs[index];

		if (token != ' ') {

			// iRect src_r = { glyph.x0, glyph.y0, glyph.x1 - glyph.x0, glyph.y1 - glyph.y0 };
			// Rect dst_r = { x + glyph.xoff, y - (src_r.h + glyph.yoff), src_r.w, src_r.h };

			iRect src_r = { glyph.x, glyph.y, glyph.w, glyph.h };
			Rect dst_r = { x + glyph.x_bearing, y + glyph.y_bearing, src_r.w, src_r.h };


			D_PushQuad(rend, dst_r, src_r);
		}
		x += glyph.x_advance * D_GetScale().x;
	}
	D_EndQuads(rend);

	D_SetTexture(rend, texture_prev);
}