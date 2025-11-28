#ifndef TTF_API_H
#define TTF_API_H

// all coordinates are unscaled

typedef struct {
	i32 ascent;
	i32 descent;
	i32 line_gap;
	i32 bb_x0, bb_y0, bb_x1, bb_y1;
	f32 inv_units_per_em;
} TTF_Font;

typedef struct {
	u32 index;
	i16 x0, y0, x1, y1;
	i16 x_bearing;
	i16 x_advance;
} TTF_Glyph;

typedef struct {
	vec2 scale, offset;
} TTF_Transform;

typedef union {
	struct { i32 x0, y0, x1, y1; };
	struct { vec2i p0, p1; };
} TTF_BoundingBox;

typedef struct {
	TTF_Glyph         src;
	Image_r_u8        dst;
	TTF_Transform   xform;
	f32             error;
} TTF_Raster;


TTF_Font *ttf_load(void *data);
TTF_Glyph ttf_glyph_from_rune(TTF_Font *ttf, Rune rune);
void ttf_rasterize(TTF_Font *ttf, TTF_Raster *raster);

typedef struct {
	f32            *df;
	TTF_BoundingBox bb;
	// Todo, you're supposed to pass in the buffer!
	u32 w, h, stride;
} MSDF_Rasterizer_Output;

typedef struct {
	TTF_Glyph       glyph;
	TTF_Transform   xform;
	Image_r_u8  dest;
	f32             curve_accuracy;
} MSDF_Rasterizer_Input;

TTF_Transform ttf_make_glyph_xform_for_image_output_at_height(TTF_Font *font, TTF_Glyph glyph, f32 height);
TTF_BoundingBox ttf_get_glyph_bounding_box(TTF_Glyph gly, TTF_Transform xform);


#endif