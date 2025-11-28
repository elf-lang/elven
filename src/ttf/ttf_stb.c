#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


// Todo, allocate the font data along with this struct thing
typedef struct {
	TTF_Font       base;
	stbtt_fontinfo info;
} STB_Font;

FILEFUNC
stbtt_fontinfo *font_info_from_font(TTF_Font *font) {
	return & ((STB_Font *) font)->info;
}

APIFUNC
TTF_Font *ttf_load(void *data) {
	require(data, "no data to load ttf font from");
	/* Todo, allocation, pass in arena */
	TTF_Font *font = calloc(1, sizeof(STB_Font));
	stbtt_fontinfo *info = font_info_from_font(font);

	b32 success = stbtt_InitFont(info, data, 0);
	if (success) {
		f32 inv_units_per_em = stbtt_ScaleForMappingEmToPixels(info, 1);

		i32 bb_x0, bb_y0, bb_x1, bb_y1;
		stbtt_GetFontBoundingBox(info, &bb_x0, &bb_y0, &bb_x1, &bb_y1);

		i32 ascent, descent, line_gap;
		stbtt_GetFontVMetrics(info, &ascent, &descent, &line_gap);

		font->ascent = ascent;
		font->descent = descent;
		font->line_gap = line_gap;
		font->bb_x0 = bb_x0;
		font->bb_y0 = bb_y0;
		font->bb_x1 = bb_x1;
		font->bb_y1 = bb_y1;
		font->inv_units_per_em = inv_units_per_em;
	}
	return font;
}

APIFUNC
TTF_Glyph ttf_glyph_from_rune(TTF_Font *font, Rune rune) {
	stbtt_fontinfo *info = font_info_from_font(font);

	u32 index = stbtt_FindGlyphIndex(info, rune);

	i32 x_advance = 0, x_bearing = 0;
	stbtt_GetGlyphHMetrics(info, index, &x_advance, &x_bearing);

	i32 x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	stbtt_GetGlyphBox(info, index, &x0, &y0, &x1, &y1);

	TTF_Glyph glyph = {
		.index = index,
		.x0 = x0, .y0 = y0, .x1 = x1, .y1 = y1,
		.x_bearing = x_bearing,
		.x_advance = x_advance,
	};
	return glyph;
}

APIFUNC
void ttf_rasterize(TTF_Font *font, TTF_Raster *in) {
	ASSERT(in->dst.reso.x > 0);
	ASSERT(in->dst.reso.y > 0);
	ASSERT(in->dst.elem_stride > 0);
	ASSERT(in->dst.data != 0);

	stbtt_fontinfo *info = font_info_from_font(font);

	u32 index = in->src.index;

	stbtt_vertex *verts;
	int num_verts = stbtt_GetGlyphShape(info, index, &verts);


	stbtt__bitmap gbm = {
		.w      = in->dst.reso.x,
		.h      = in->dst.reso.y,
		.stride = in->dst.elem_stride,
		.pixels = in->dst.data,
	};

	f32 scale_x = in->xform.scale.x;
	f32 scale_y = in->xform.scale.y;
	i32 invert = scale_y < 0;
	scale_y = fabsf(scale_y);

	f32 shift_x = 0.f;
	f32 shift_y = 0.f;

	// same as stb
	i32 ix0 = (i32) floor(in->src.x0 * scale_x + shift_x);
	i32 iy0 = (i32) floor(in->src.y0 * scale_y + shift_y);
	i32 ix1 = (i32) ceil (in->src.x1 * scale_x + shift_x);
	i32 iy1 = (i32) ceil (in->src.y1 * scale_y + shift_y);

	i32 x_off = + ix0;
	i32 y_off = - iy1;

	ASSERT(ix1 - ix0 <= in->dst.reso.x);
	ASSERT(iy1 - iy0 <= in->dst.reso.y);


	f32 flatness = 0.25f / MIN(scale_x, scale_y);

	i32 winding_count = 0;
	i32 *winding_lengths = NULL;
	stbtt__point *windings = stbtt_FlattenCurves(verts, num_verts, flatness, &winding_lengths, &winding_count, info->userdata);

	if (windings) {
		stbtt__rasterize(&gbm, windings, winding_lengths, winding_count, scale_x, scale_y, shift_x, shift_y, x_off, y_off, invert, info->userdata);

		STBTT_free(winding_lengths, info->userdata);
		STBTT_free(windings, info->userdata);
	}

	STBTT_free(verts, info->userdata);
}
