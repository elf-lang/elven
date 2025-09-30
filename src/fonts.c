// todo: this is a work in progress


// todo: I want to allow the user to allocate pages themselves,
// then allocate subrectangles from there.
// To allow for custom fonts...


#define TOFONTHANDLE(ptr) ((FONT_HANDLE){ptr})
#define TOFONT(h) ((Loaded_Font *) (h))




typedef struct {
	// todo: sparse mapping!
	u32             rune;
	Font_Glyph_Data data;
} Glyph_Entry;


typedef struct {
	b32  dirty;
	RID  texture;
	// todo: better packer!
	int  row_height_so_far;
	int  next_x;
	int  next_y;
	int  w, h;
	u8   rastercache[];
} Font_Page;


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
	RID          		      texture;

	TTF_RUNTIME_FACE      *ttf_face;

	// hmmmm...
	int                      height;
	int                  num_glyphs;
	Glyph_Entry       	  glyphs[];
};







static inline FONT_HANDLE NewFont(int num_glyphs) {
	Loaded_Font *font = calloc(1, sizeof(*font) + sizeof(*font->glyphs) * num_glyphs);
	font->num_glyphs = num_glyphs;
	return font;
}



void SetFontTexture(FONT_HANDLE font, RID texture) {
	TOFONT(font)->texture = texture;
}



void SetFontGlyph(FONT_HANDLE font, int rune, Font_Glyph_Data data) {
	TOFONT(font)->glyphs[rune].rune = rune;
	TOFONT(font)->glyphs[rune].data = data;
}









Font_Page *g_page;





static Font_Page *new_font_page(int w, int h) {
	Font_Page *page = calloc(1, sizeof(*page) + sizeof(*page->rastercache) * w * h);
	page->w = w;
	page->h = h;
	return page;
}





// todo: better packer!
static int allocate_page_rect(Font_Page *page, int *x, int *y, int w, int h) {
	ASSERT(w > 0 && w <= page->w);
	ASSERT(h > 0 && h <= page->h);

	int success = false;

	if (page->next_x + w > page->w) {
		page->next_y += page->row_height_so_far;
		page->next_x = 0;
		page->row_height_so_far = 0;
	}

	*x = page->next_x;
	*y = page->next_y;

	if (page->next_y + h <= page->h) {
		page->next_x += w;
		if (page->row_height_so_far < h) {
			page->row_height_so_far = h;
		}
		success = true;
	}


	return success;
}





static void request_glyph(Loaded_Font *font, u32 rune) {
	ASSERT(rune != 0);

	// todo:
	if (!g_page) g_page = new_font_page(4096, 4096);
	Font_Page *page = g_page;

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


	int glyph_w = ceil((glyph.x1 - glyph.x0) * scale);
	int glyph_h = ceil((glyph.y1 - glyph.y0) * scale);

	if (glyph_w <= 0) return;
	if (glyph_h <= 0) return;



	int glyph_x, glyph_y;
	allocate_page_rect(page, &glyph_x, &glyph_y, glyph_w, glyph_h);

	ASSERT(glyph_x>=0 && glyph_x+glyph_w<=page->w);
	ASSERT(glyph_y>=0 && glyph_y+glyph_h<=page->h);

	int index = glyph.index;

	TTF_RASTER_PARAMS raster;
	raster.target  = glyph;
	raster.scale_x = scale;
	raster.scale_y = scale;
	raster.output.buffer = page->rastercache;
	raster.output.stride = page->w * sizeof(*page->rastercache);
	raster.output.x = glyph_x;
	raster.output.y = glyph_y;
	raster.output.w = glyph_w;
	raster.output.h = glyph_h;

	TTF_RasterizeGlyph(ttf, &raster);

	entry->rune = rune;
	entry->data.x = glyph_x;
	entry->data.y = glyph_y;
	entry->data.w = glyph_w;
	entry->data.h = glyph_h;
	entry->data.x_advance = glyph.x_advance * scale;
	entry->data.x_bearing = glyph.x_bearing * scale;
	entry->data.y_bearing = glyph.y0        * scale;
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
		request_glyph(font, i);
	}

	return TOFONTHANDLE(font);
}





// todo: remove text scale from here!

// todo: remove this routine at all, because what we
// really want is something like, get text quads...
f32 MeasureText(FONT_HANDLE hfont, const char *text) {
	Loaded_Font *font = TOFONT(hfont);

	f32 width = 0.0;


	while (*text) {
		Glyph_Entry glyph = font->glyphs[*text ++];
		width += glyph.data.x_advance;
	}

	return width * gd.text_scale;
}

//
//
//
//

//
// todo: remove from here...
//
void DrawText(FONT_HANDLE hfont, f32 x, f32 y, const char *text) {
	Loaded_Font *font = TOFONT(hfont);


	// why would you even call
	assert(text);


	Color color = D_GetColor();


	// RID restore_texture = R_GetTexture(gd.rend);

	if (!g_page->texture) {
		g_page->texture = R_InstallTexture(gd.rend, FORMAT_R8_UNORM, (vec2i){ g_page->w, g_page->h }, g_page->rastercache);
	}

	D_SetTexture(g_page->texture);
	// D_SetTexture(font->texture);
	D_BeginQuads();

	while (*text) {
		i32 chr = *text ++;
		Font_Glyph_Data glyph = font->glyphs[chr].data;

		if (chr != ' ') {
			iRect src_r = { glyph.x, glyph.y, glyph.w, glyph.h };
			Rect dst_r = {
				x + glyph.x_bearing,
				y + glyph.y_bearing,
				src_r.w  * gd.text_scale,
				src_r.h  * gd.text_scale
			};
			D_PushQuad(dst_r, src_r);
		}
		x += glyph.x_advance * gd.text_scale;
	}
	D_EndQuads();

	// D_SetTexture(restore_texture);
}




#if 0
	TTF_GLYPH *ttf_glyphs = calloc(sizeof(*ttf_glyphs), 256);
	for (int i = 32; i < 127; i ++) {
		ttf_glyphs[i] = TTF_QueryGlyph(ttf_font, i);
	}


	f32 scale = (f32) font_size / (f32) (ttf_font->ascent - ttf_font->descent);



	int atlas_size = 4096;
	u8 *temp_atlas = calloc(atlas_size * atlas_size, 1);




	Pack_Rect rects[256] = {};
	for (int i = 32; i < 127; i ++) {
		TTF_GLYPH glyph = ttf_glyphs[i];
		rects[i].data = i;
		rects[i].w = ceil((glyph.x1 - glyph.x0) * scale);
		rects[i].h = ceil((glyph.y1 - glyph.y0) * scale);
	}
	RunSimplePacker(rects + 32, 127 - 32, atlas_size, atlas_size);


	Loaded_Font *font = NewFont(256);
	font->ttf_face = ttf_font;
	font->ttf_glyphs = ttf_glyphs;


	TTF_RASTER_PARAMS raster = {};
	for (int i = 32; i < 127; i ++) {
		if (rects[i].w<=0) continue;
		if (rects[i].h<=0) continue;
		ASSERT(rects[i].x>=0 && rects[i].x+rects[i].w<=atlas_size);
		ASSERT(rects[i].y>=0 && rects[i].y+rects[i].h<=atlas_size);

		int index = rects[i].data;

		raster.target = ttf_glyphs[index];
		raster.output.buffer = temp_atlas;
		raster.output.stride = atlas_size;
		raster.output.x = rects[i].x;
		raster.output.y = rects[i].y;
		raster.output.w = rects[i].w;
		raster.output.h = rects[i].h;
		raster.scale_x = scale;
		raster.scale_y = scale;

		TTF_RasterizeGlyph(ttf_font, &raster);

		Font_Glyph_Data data = {
			.x = rects[i].x,
			.y = rects[i].y,
			.w = rects[i].w,
			.h = rects[i].h,
			.x_advance = raster.target.x_advance * scale,
			.x_bearing = raster.target.x_bearing * scale,
			.y_bearing = raster.target.y0 * scale,
		};
		SetFontGlyph(font, i, data);
	}
	font->texture = R_InstallTexture(gd.rend, FORMAT_R8_UNORM, (vec2i){ atlas_size, atlas_size }, temp_atlas);
#endif
