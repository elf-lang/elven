#ifndef FONTS_API_H
#define FONTS_API_H

// Todo, there's no need for the font system to know about the renderer at all, instead ...
// The font system only has to track image buffers, and provide a simple way to attach a user
// pointer to each buffer, on our side, we check if the buffer is tagged as dirty, if so we
// update the texture (stored in the user pointer) ...

enum {
	MAX_FONT_PAGES = 16
};


#include "ttf_api.h"

typedef struct F_GlyphPage F_GlyphPage;

// Note, height is stored as integer for stable comparisons!
// Note, we use a raw pointer here for simplicity, but we may want to switch to a 64-bit
// handle, for safety in case the font is freed or something ...
typedef struct {
	Rune      rune;
	i32       height;
	TTF_Font *font;
} F_GlyphKey;

typedef struct {
	UV_Coords src;
	rect_f32  dst;
} F_Quad;

// Quads from the same page
typedef struct F_Pass F_Pass;
struct F_Pass {
	F_Pass        *prox;
	F_GlyphPage   *page;
	i32          nquads;
	F_Quad        quads[];
};

typedef struct {
	vec2       dim;
	F_Pass *passes;
} F_Run;


struct F_GlyphPage {
// --- user read/write ---
	void             *user;
	b32              dirty;
// --- transient per-run build state ---
	vec2i        alloc_pos;
	i32          alloc_row;
	Image_r_u8       atlas;
// --- transient per-run build state ---
	F_Pass           *pass;
	i32              nquads;
};

typedef struct {
	F_GlyphPage *page;
	i32 w, h;
	// Todo, insted replace this with the bounding box scaled ... that way it works
	// regardless of rendering system ...
	f32 x_bearing, y_bearing;
	f32 x_advance, user_data;
	UV_Coords src;
} F_GlyphCache;



typedef struct {
	F_GlyphKey   key;
	F_GlyphCache cache;
} F_GlyphEntry;

typedef struct {
	u32          usage;
	u32          sparse_size;
	F_GlyphEntry sparse[];
} F_GlyphTable;

typedef struct {
	Stack         stack;
	F_GlyphTable *table;

	// Todo, this can made into a linked list!
	F_GlyphPage   pages[MAX_FONT_PAGES];
	int          npages;

	f32          supersampling;
} F_Cache;

F_Cache *F_NewFontCache();
F_Run F_BuildTextRun(F_Cache *fc, Stack stack, TTF_Font *font, i32 height, String string);

#endif