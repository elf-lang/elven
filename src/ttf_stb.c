

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


#define get_info(lib) ((stbtt_fontinfo *)(lib)->extension)



static TTF_RUNTIME_FACE *TTF_InitRuntimeFace(void *data) {

	stbtt_fontinfo *info;
	TTF_RUNTIME_FACE *lib = calloc(sizeof(*lib) + sizeof(*info), 1);

	info = get_info(lib);


	int success = stbtt_InitFont(info, data, 0);

	lib->inv_units_per_em = stbtt_ScaleForMappingEmToPixels(info, 1);
	stbtt_GetFontBoundingBox(info, &lib->bb_x0, &lib->bb_y0, &lib->bb_x1, &lib->bb_y1);

	stbtt_GetFontVMetrics(info, &lib->ascent, &lib->descent, &lib->line_gap);

	return lib;
}



static TTF_GLYPH TTF_QueryGlyph(TTF_RUNTIME_FACE *ttf, int charcode) {
	stbtt_fontinfo *info = get_info(ttf);

	TTF_GLYPH glyph = {};

	int x_advance, x_bearing;
	int x0, y0, x1, y1;

	glyph.index = stbtt_FindGlyphIndex(info, charcode);
	stbtt_GetGlyphHMetrics(info, glyph.index, &x_advance, &x_bearing);

	stbtt_GetGlyphBox(info, glyph.index, &x0, &y0, &x1, &y1);


	glyph.x_advance = x_advance;
	glyph.x_bearing = x_bearing;
	glyph.x0 = x0;
	glyph.y0 = y0;
	glyph.x1 = x1;
	glyph.y1 = y1;
	return glyph;
}



static TTF_SHAPE_VERTEX *TTF_GetGlyphVertices(TTF_RUNTIME_FACE *ttf, TTF_GLYPH_INDEX index, int *num_verts) {
	stbtt_fontinfo *info = get_info(ttf);

	stbtt_vertex *vertices = 0;
	*num_verts = stbtt_GetGlyphShape(info, index.index, &vertices);
	return (TTF_SHAPE_VERTEX *) vertices;
}



static void TTF_RasterizeGlyph(TTF_RUNTIME_FACE *ttf, TTF_RASTER_PARAMS *raster) {

	stbtt_fontinfo *info = get_info(ttf);




	stbtt_vertex *vertices;
	int num_verts = stbtt_GetGlyphShape(info, raster->target.index, &vertices);

	float   scale_x = raster->scale_x;
	float   scale_y = raster->scale_y;
	float   shift_x = raster->shift_x;
	float   shift_y = raster->shift_y;


	stbtt__bitmap gbm;
	{
		TTF_RASTER_PIXEL *source = raster->output.buffer;
		source += raster->output.y * raster->output.stride;
		source += raster->output.x;

		gbm.w      = raster->output.w;
		gbm.h      = raster->output.h;
		gbm.stride = raster->output.stride;
		gbm.pixels = source;
	}

	int x_off  =  floor(raster->target.x0 * scale_x);
	int y_off  = - ceil(raster->target.y1 * scale_y);


	if (gbm.w && gbm.h) {
		float flatness = 0.35f / MIN(scale_x, scale_y);

		// convert curves to points
		int  winding_count = 0;
   	int *winding_lengths = NULL;
   	stbtt__point *windings = stbtt_FlattenCurves(vertices, num_verts, flatness, &winding_lengths, &winding_count, info->userdata);


		int invert = true;

		// rasterize points
   	if (windings) {
   		stbtt__rasterize(&gbm, windings, winding_lengths, winding_count, scale_x, scale_y, shift_x, shift_y, x_off, y_off, invert, info->userdata);

	      STBTT_free(winding_lengths, info->userdata);
	      STBTT_free(windings, info->userdata);
   	}
	}

	STBTT_free(vertices, info->userdata);
}