#define USE_FREETYPE

// todo: we need a packer


#if defined(USE_FREETYPE)
#define _CRT_SECURE_NO_WARNINGS
#include "ft2build.h"
#include FT_FREETYPE_H
#endif


#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#if 0
// <3
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#endif


#include "elements.h"
#include "platform.h"
#include "renderer.h"
#include "drawstate.h"


typedef struct {
	unsigned short x, y, w, h;
	short x_bearing, y_bearing;
	short x_advance, unused;
} BF_Glyph;


enum {
	FONTS_CAPACITY       = 16,
	FONT_NAME_CAPACITY   = 128,
	FONT_GLYPHS_CAPACITY = 256,
};



typedef struct D_FONT D_FONT;
struct D_FONT {
	RID        texture;
	char       name[FONT_NAME_CAPACITY];
	BF_Glyph   glyphs[FONT_GLYPHS_CAPACITY];
};


enum {
	BFF_OPT_PIXEL_FONT = 1,
	BFF_OPT_SIZE_EM    = 2,
};



static int RunSimplePacker(BF_Glyph *glyphs, int num_glyphs, int padding, int atlas_width, int atlas_height) {
	atlas_width -= padding * 2;
	atlas_height -= padding * 2;

	int next_glyph_x = padding;
	int next_glyph_y = padding;

	int row_height_so_far = 0;

	BF_Glyph *glyph;
	for (glyph = glyphs; glyph < glyphs + num_glyphs; glyph ++) {
		if (!glyph->w || !glyph->h) continue;

		if (next_glyph_x + glyph->w + padding > atlas_width) {
			next_glyph_y += row_height_so_far + padding;
			row_height_so_far = 0;
			next_glyph_x = 0;
		}
		if (next_glyph_y + glyph->h + padding > atlas_height) {
			return 0;
		}
		if (row_height_so_far < glyph->h + padding) {
			row_height_so_far = glyph->h + padding;
		}

		glyph->x = next_glyph_x;
		glyph->y = next_glyph_y;

		next_glyph_x += glyph->w + padding;
	}

	return next_glyph_y;
}


D_FONT *InstallFont(char *path, int font_size)
{
	D_FONT *font = calloc(1, sizeof(*font));

	int padding = 1;

#if defined(USE_FREETYPE)
	int render_mode = FT_RENDER_MODE_NORMAL; // _LCD;

	int width_multiplier = 1;
	if (render_mode == FT_RENDER_MODE_LCD) {
		width_multiplier = 3;
	}

	FT_Library library;
	FT_Face face;

	int error = FT_Init_FreeType(&library);
	assert(!error);

	error = FT_New_Face(library, path, 0, &face);
	assert(!error);

	// error = FT_Set_Char_Size(face,0,size << 6,0,0);
	// if (error) exit(error);
	error = FT_Set_Pixel_Sizes(face, 0, font_size);
	assert(!error);

	// FT_LOAD_MONOCHROME | FT_LOAD_NO_HINTING

#endif


	int num_glyphs = 127 - 32;

	BF_Glyph *glyphs = font->glyphs;


	int strip_width = 1;
	int strip_height = 1;

#if defined(USE_FREETYPE)
	FT_Bitmap *bmp = & face->glyph->bitmap;
	for (int i = 0; i < num_glyphs; i ++) {
		int character = i + 32;

		int glyph_index = FT_Get_Char_Index(face, character);
		if (glyph_index == 0) continue;

		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT|FT_LOAD_BITMAP_METRICS_ONLY);
		assert(!error);

		glyphs[character].w = character == ' ' ? 1 : bmp->width * width_multiplier;
		glyphs[character].h = character == ' ' ? 1 : bmp->rows;
		assert(glyphs[character].w != 0);
		assert(glyphs[character].h != 0);

		strip_width += glyphs[character].w + padding;
		strip_height += glyphs[character].h + padding;
	}
#endif

	int atlas_area_approx = sqrt(strip_width * strip_height);
	int stride = pow(2, ceil(log2(atlas_area_approx)));

	int atlas_width = stride;
	int atlas_height = stride;

	int pack_result = RunSimplePacker(glyphs + 32, num_glyphs, padding, atlas_width, atlas_height);
	assert(pack_result);


	// todo: determine how much of the thing we actually used up
	unsigned char *temp_atlas = calloc(atlas_width * atlas_height, 1);

#if defined(USE_FREETYPE)
	for (int i = 0; i < num_glyphs; i ++) {
		int character = 32 + i;
		int glyph_index = FT_Get_Char_Index(face, character);
		if (glyph_index == 0) continue;

		error = FT_Load_Glyph(face, glyph_index, 0);
		assert(!error);

		if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
			error = FT_Render_Glyph(face->glyph, render_mode);
			assert(!error);
		}

		FT_GlyphSlotRec *glyph_info = face->glyph;
		glyphs[character].x_bearing = glyph_info->bitmap_left;
		glyphs[character].y_bearing = glyph_info->bitmap_top;
		glyphs[character].x_advance = glyph_info->linearHoriAdvance >> 16;
		int dst_x = glyphs[character].x;
		int dst_y = glyphs[character].y;

		if (render_mode == FT_RENDER_MODE_MONO) {
			for (int y = 0; y < bmp->rows; y ++) {
				for (int x = 0; x < bmp->width; x ++) {
					int src = bmp->buffer[(x >> 3) + bmp->pitch * y] >> ((x & 7) ^ 7);
					temp_atlas[(dst_x + x) + (dst_y + y) * stride] = (src & 1) * 255;
				}
			}
		} else {
			for (int y = 0; y < bmp->rows; y ++) {
				void *dst = temp_atlas + dst_x + (dst_y + y) * stride;
				void *src = bmp->buffer + bmp->pitch * y;
				memcpy(dst, src, bmp->width);
			}
		}
	}
#endif

	font->texture = R_InstallTexture(gd.rend, FORMAT_R8_UNORM, (vec2i){ atlas_width, atlas_height }, temp_atlas);
	return font;
}




f32 MeasureText(const char *text) {
	f32 width = 0.0;

	D_FONT *font = D_GetFont();

	while (*text) {
		i32 token = *text ++;
		i32 index = token - 32;

		BF_Glyph glyph = font->glyphs[index];
		width += glyph.x_advance;
	}

	return width * D_GetScale().x;
}


//
// todo: proper text rendering...
//
void D_DrawText(f32 x, f32 y, const char *text) {
	// why would you even call
	assert(text);

	D_FONT *font = D_GetFont();

	Color color = D_GetColor();


	RID restore_texture = R_GetTexture(gd.rend);

	D_SetTexture(font->texture);
	D_BeginQuads();

	while (*text) {
		i32 token = *text ++;
		// i32 index = token - 32;
		i32 index = token;
		BF_Glyph glyph = font->glyphs[index];

		if (token != ' ') {

			// iRect src_r = { glyph.x0, glyph.y0, glyph.x1 - glyph.x0, glyph.y1 - glyph.y0 };
			// Rect dst_r = { x + glyph.xoff, y - (src_r.h + glyph.yoff), src_r.w, src_r.h };

			iRect src_r = { glyph.x, glyph.y, glyph.w, glyph.h };
			Rect dst_r = { x + glyph.x_bearing, y + (glyph.y_bearing - glyph.h), src_r.w, src_r.h };


			D_PushQuad(dst_r, src_r);
		}
		x += glyph.x_advance * D_GetScale().x;
	}
	D_EndQuads();

	D_SetTexture(restore_texture);
}