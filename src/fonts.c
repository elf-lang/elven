//
// todo: this is a work in progress
//
//	The font system has to be simple, but also precise enough that
// the user gets good performance out of it.
//


#define TOFONTHANDLE(ptr) ((FONT_HANDLE){ptr})
#define TOFONT(h) ((Loaded_Font *) (h))


enum {
	MAX_FONT_PAGES = 16
};


typedef struct {
	int  row_height_so_far;
	int  next_x;
	int  next_y;
	vec2i reso;
	u8    data[];
} Font_Page_Image;

typedef struct {
	b32  dirty;
	RID  texture;
	Font_Page_Image *image;
	// idk prob remove from here
	i32 run_index;
	i32 num_quads;
} Font_Page;

typedef struct {
	// todo: sparse mapping!
	u32             rune;
	Font_Page      *page;
	struct {
		// todo: keep in UV space!
		short w, h;
		short x_bearing, y_bearing;
		short x_advance, user_data;
		UV_Coords src;
	};
} Glyph_Entry;




typedef struct {
	// input
	int w, h;

	// output
	int x, y;

	// constant
	int data;
} Pack_Rect;


typedef struct Loaded_Font Loaded_Font;
struct Loaded_Font {
	TTF_RUNTIME_FACE      *ttf_face;

	// hmmmm...
	int                      height;
	int                  num_glyphs;
	Glyph_Entry       	  glyphs[];
};



struct {
	Font_Page  pages[MAX_FONT_PAGES];
	int       npages;
	u8        *tr_zone_mem;
	i64        tr_zone_min;
	i64        tr_zone_max;
} global g_fonts;

static void trz_clear() {
	g_fonts.tr_zone_min = 0;
}

static void *trz_alloc(int size) {
	if (g_fonts.tr_zone_max == 0) {
		g_fonts.tr_zone_max = MEGABYTES(32);
		g_fonts.tr_zone_min = 0;
		g_fonts.tr_zone_mem = malloc(g_fonts.tr_zone_max);
	}
	ASSERT(g_fonts.tr_zone_min + size <= g_fonts.tr_zone_max);
	void *mem = g_fonts.tr_zone_mem + g_fonts.tr_zone_min;
	g_fonts.tr_zone_min += size;
	return mem;
}


static void *trz_calloc(int size) {
	void *mem = trz_alloc(size);
	memset(mem, 0, size);
	return mem;
}


static Font_Page *NewFontPage(int w, int h) {
	Font_Page_Image *image = calloc(1, sizeof(*image) + sizeof(*image->data) * w * h);
	image->reso.x = w;
	image->reso.y = h;

	i32 index = g_fonts.npages ++;
	Font_Page *page = & g_fonts.pages[index];
	page->image = image;
	return page;
}

FONT_HANDLE NewFont(int num_glyphs) {
	Loaded_Font *font = calloc(1, sizeof(*font) + sizeof(*font->glyphs) * num_glyphs);
	font->num_glyphs = num_glyphs;
	return font;
}

// todo: better packer!
static int RequestFontPageSpace(Font_Page *page, int *x, int *y, int w, int h) {
	Font_Page_Image *image = page->image;
	ASSERT(w > 0 && w <= image->reso.x);
	ASSERT(h > 0 && h <= image->reso.y);

	int success = false;

	if (image->next_x + w > image->reso.x) {
		image->next_y += image->row_height_so_far;
		image->next_x = 0;
		image->row_height_so_far = 0;
	}

	*x = image->next_x;
	*y = image->next_y;

	if (image->next_y + h <= image->reso.y) {
		image->next_x += w;
		if (image->row_height_so_far < h) {
			image->row_height_so_far = h;
		}
		success = true;
	}


	return success;
}

static Font_Page *RequestFontPage() {
	// todo:
	if (!g_fonts.npages) {
		NewFontPage(4096, 4096);
	}
	return g_fonts.pages;
}

// so this is to support custom fonts and stuff...
void SetFontGlyph(FONT_HANDLE font, ADD_FONT_GLYPH params) {
	Font_Page *page = RequestFontPage();

	Glyph_Entry *entry = & TOFONT(font)->glyphs[params.rune];
	entry->rune = params.rune;

	int dst_x, dst_y;
	if (RequestFontPageSpace(page, &dst_x, &dst_y, params.src_w, params.src_h))
	{
		f32 inv_w = 1.0 / page->image->reso.x;
		f32 inv_h = 1.0 / page->image->reso.y;
		entry->rune = params.rune;
		entry->page = page;
		entry->w = params.src_w;
		entry->h = params.src_h;
		entry->src.u0 = dst_x * inv_w;
		entry->src.v0 = dst_y * inv_h;
		entry->src.u1 = (dst_x + params.src_w) * inv_w;
		entry->src.v1 = (dst_y + params.src_h) * inv_h;
		entry->x_bearing = params.x_bearing;
		entry->y_bearing = params.y_bearing;
		entry->x_advance = params.x_advance;


		i32 src_x = params.src_x;
		i32 src_y = params.src_y;

		Font_Page_Image *dst_i = page->image;
		Image *src_i = params.src;

		TTF_RASTER_PIXEL *dst = dst_i->data;
		Color            *src = src_i->data;

		int dst_stride = dst_i->reso.x;
		int src_stride = src_i->reso.x;

		TTF_RASTER_PIXEL *dst_row;
		Color            *src_row;

		dst_row = dst + dst_stride * dst_y + dst_x;
		src_row = src + src_stride * src_y + src_x;

		// todo: why is raster cache gray scale... what if we want color
		// fonts... and it would remove this ambiguous conversion here...
		for (int y = 0; y < params.src_h; y ++) {
			for (int x = 0; x < params.src_w; x ++) {
				dst_row[x] = src_row[x].a;
			}
			dst_row += dst_stride;
			src_row += src_stride;
		}
	}
}

static void RequestGlyph(Loaded_Font *font, u32 rune) {
	ASSERT(font);
	ASSERT(font->ttf_face);
	ASSERT(rune != 0);

	Font_Page *page = RequestFontPage();

	TTF_RUNTIME_FACE *ttf = font->ttf_face;

	// todo: !
	Glyph_Entry *entry = & font->glyphs[rune];
	if (entry->rune == rune) {
		return;
	}

	// why does the font store the height!
	f32 scale = (f32) font->height / (f32) (ttf->ascent - ttf->descent);

	// todo: this should be fast enough right...
	TTF_GLYPH glyph = TTF_QueryGlyph(font->ttf_face, rune);


	int g_w = ceil((glyph.x1 - glyph.x0) * scale);
	int g_h = ceil((glyph.y1 - glyph.y0) * scale);

	if (g_w <= 0) return;
	if (g_h <= 0) return;



	int g_x, g_y;
	RequestFontPageSpace(page, &g_x, &g_y, g_w, g_h);
	//	ASSERT(g_x>=0 && g_x+g_w<=page->w);
	//	ASSERT(g_y>=0 && g_y+g_h<=page->h);

	int index = glyph.index;

	Font_Page_Image *dst = page->image;

	TTF_RASTER_PARAMS raster;
	raster.target  = glyph;
	raster.scale_x = scale;
	raster.scale_y = scale;
	raster.output.buffer = dst->data;
	raster.output.stride = dst->reso.x * sizeof(*dst->data);
	raster.output.x = g_x;
	raster.output.y = g_y;
	raster.output.w = g_w;
	raster.output.h = g_h;

	TTF_RasterizeGlyph(ttf, &raster);

	f32 inv_w = 1.0 / page->image->reso.x;
	f32 inv_h = 1.0 / page->image->reso.y;

	entry->rune = rune;
	entry->page = page;
	entry->w = g_w;
	entry->h = g_h;
	entry->src.u0 = g_x * inv_w;
	entry->src.v0 = g_y * inv_h;
	entry->src.u1 = (g_x + g_w) * inv_w;
	entry->src.v1 = (g_y + g_h) * inv_h;
	entry->x_advance = glyph.x_advance * scale;
	entry->x_bearing = glyph.x_bearing * scale;
	entry->y_bearing = glyph.y0        * scale;
}

static int RunSimplePacker(Pack_Rect *rects, int num_rects, int max_x, int max_y) {
	int next_x = 0;
	int next_y = 0;
	int row_height_so_far = 0;

	Pack_Rect *r;
	for (r = rects; r < rects + num_rects; r ++) {
		if (!r->w || !r->h) continue;

		if (next_x + r->w > max_x) {
			next_y += row_height_so_far;
			row_height_so_far = 0;
			next_x = 0;
		}
		if (next_y + r->h > max_y) {
			return 0;
		}
		if (row_height_so_far < r->h) {
			row_height_so_far = r->h;
		}

		r->x = next_x;
		r->y = next_y;

		next_x += r->w;
	}

	return next_y;
}

FONT_HANDLE LoadTrueTypeFont(void *data, int font_size)
{
	TTF_RUNTIME_FACE *ttf_font = TTF_InitRuntimeFace(data);

	Loaded_Font *font = NewFont(256);
	font->height = font_size;
	font->ttf_face = ttf_font;

	for (int i = 32; i < 127; ++i) {
		RequestGlyph(font, i);
	}

	return TOFONTHANDLE(font);
}

// todo: remove this routine at all, because what we
// really want is something like, get text quads...
f32 MeasureText(FONT_HANDLE hfont, const char *text) {
	Loaded_Font *font = TOFONT(hfont);

	f32 width = 0.0;


	while (*text) {
		Glyph_Entry glyph = font->glyphs[*text ++];
		width += glyph.x_advance;
	}

	return width;
}

// todo:
static inline bool is_rune_visible(Rune rune) {
	return rune != ' ' && rune != '\t' && rune != '\n';
}

Draw_Text_Pass *DrawText(FONT_HANDLE hfont, const char *text, int textl) {
	// why would you even call
	assert(text);
	Loaded_Font *font = TOFONT(hfont);

	// todo: what's faster, doing this, or just sorting...

	trz_clear();

	for (int i = 0; i < g_fonts.npages; ++ i) {
		g_fonts.pages[i].run_index = 0;
		g_fonts.pages[i].num_quads = 0;
	}

	int num_runs = 0;
	for (int i = 0; i < textl; ++ i) {
		Rune rune = text[i];
		if (is_rune_visible(rune)) {
			Glyph_Entry glyph = font->glyphs[rune];
			// todo: this should never happen!
			if (glyph.rune != rune) continue;

			ASSERT(glyph.page < g_fonts.pages + g_fonts.npages);

			if (!glyph.page->num_quads) {
				glyph.page->run_index = num_runs;
				num_runs += 1;
			}
			glyph.page->num_quads ++;
		}
	}

	// each run is allocated bellow, linearly
	Draw_Text_Pass *pass = trz_alloc(sizeof(*pass));
	pass->nruns = num_runs;

	for (int i = 0; i < g_fonts.npages; ++ i) {
		Font_Page *page = & g_fonts.pages[i];
		if (page->num_quads) {
			if (!page->texture) {
				vec2i reso = page->image->reso;
				TTF_RASTER_PIXEL *data = page->image->data;
				page->texture = R_InstallTexture(gd.rend, FORMAT_R_U8, reso, data);
			}
			Text_Run *run = trz_alloc(sizeof(*pass->runs) + sizeof(*pass->runs->quads) * page->num_quads);
			run->texture = page->texture;
			run->nquads = 0;
		}
	}

	f32 x = 0;
	for (int i = 0; i < textl; ++ i) {
		Rune rune = text[i];
		Glyph_Entry glyph = font->glyphs[rune];
		if (is_rune_visible(rune)) {
			if (glyph.rune != rune) continue;
			i32 run_index = glyph.page->run_index;
			ASSERT(run_index < num_runs);
			Text_Run *run = & pass->runs[run_index];
			Text_Quad *quad = & run->quads[run->nquads ++];
			quad->src = glyph.src;
			quad->dst = (Rect) { x + glyph.x_bearing, glyph.y_bearing, glyph.w, glyph.h };
		}
		x += glyph.x_advance;
	}

	ASSERT(pass->nruns == num_runs);
	return pass;
}

