

typedef struct {
	// highest position relative to baseline
	int         ascent;

	// lowest position relative to baseline
	int        descent;

	// additional line space
	int       line_gap;

	int         bb_x0;
	int         bb_y0;
	int         bb_x1;
	int         bb_y1;

	// 1 / units_per_em
	float inv_units_per_em;

	u8          extension[];
} TTF_RUNTIME_FACE;


typedef struct {
	f32 x, y;
} TTF_SHAPE_POINT;


enum {
	TTF_VMOVE = 1,
   TTF_VLINE,
   TTF_VCURVE,
   TTF_VCUBIC
};

typedef struct
{
   i16 x, y, cx, cy, cx1, cy1;
   u8  type, padd;
} TTF_SHAPE_VERTEX;


typedef struct {
	int index;
} TTF_GLYPH_INDEX;

typedef struct {
	int index;

	// this is the bounding box of the glyph in unscaled
	// coordinates!
	short x0, x1, y0, y1;

	// unscaled coordinates!
	short x_advance, x_bearing;
} TTF_GLYPH;



typedef u8 TTF_RASTER_PIXEL;



typedef struct {
	int               x, y, w, h;
	int               stride;
	TTF_RASTER_PIXEL *buffer;
} TTF_RASTER_SURFACE;




typedef struct {
	TTF_GLYPH          target;
	TTF_RASTER_SURFACE output;

	float scale_x;
	float scale_y;
	float shift_x;
	float shift_y;
} TTF_RASTER_PARAMS;




TTF_RUNTIME_FACE *TTF_InitRuntimeFace(void *data);
TTF_GLYPH TTF_QueryGlyph(TTF_RUNTIME_FACE *lib, int charcode);
void TTF_RasterizeGlyph(TTF_RUNTIME_FACE *lib, TTF_RASTER_PARAMS *raster);
