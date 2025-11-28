#include "base.h"
#include "ttf_api.h"

APIFUNC
TTF_Transform ttf_make_glyph_xform_for_image_output_at_height(TTF_Font *font, TTF_Glyph glyph, f32 height) {
	f32 scale = (f32) height / (f32) (font->ascent - font->descent);
	TTF_Transform xform;
	xform.scale = v2(scale, - scale);
	xform.offset = v2(- glyph.x0 * scale, glyph.y1 * scale);
	return xform;
}

APIFUNC
TTF_BoundingBox ttf_get_glyph_bounding_box(TTF_Glyph gly, TTF_Transform xform) {
	TTF_BoundingBox result;
	result.x0 = (i32) floor(gly.x0 * xform.scale.x + xform.offset.x);
	result.y0 = (i32) floor(gly.y0 * xform.scale.y + xform.offset.y);
	result.x1 = (i32) ceil (gly.x1 * xform.scale.x + xform.offset.x);
	result.y1 = (i32) ceil (gly.y1 * xform.scale.y + xform.offset.y);
	return result;
}

#include "ttf_stb.c"