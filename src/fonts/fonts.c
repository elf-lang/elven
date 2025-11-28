// todo: proper text layouts...
#include "base.h"
#include "ttf_api.h"
#include "fonts_api.h"

#include "fonts.h"

////////////////////////////////////////
// GLYPH TABLE

FILEFUNC
F_GlyphKey make_glyph_key(Rune rune, TTF_Font *font, i32 height) {
	F_GlyphKey key = { .rune = rune, .height = height, .font = font };
	return key;
}

FILEFUNC
void check_glyph_key(F_GlyphKey key) {
	require(key.rune    != 0, "invalid rune");
	require(key.height   > 0, "invalid height");
	require(key.font    != 0, "invalid family");
}

FILEFUNC
b32 do_glyph_keys_match(F_GlyphKey x, F_GlyphKey y) {
	return x.rune == y.rune && x.height == y.height && x.font == y.font;
}

typedef struct {
	b32          status;
	F_GlyphCache *cache;
} F_GlyphQuery;

enum {
	// * WAS MISSING
	STATUS_MISSING = 0,
	STATUS_PRESENT = 1,
};

// Todo, log statics
// Todo, experiment with robin-hood hashing
// Todo, experiment with different hash functions

FOLDERFUNC
F_GlyphQuery F_QueryGlyphCache(F_Cache *fc, F_GlyphKey key) {
	check_glyph_key(key);
	b32 status = STATUS_MISSING;

	_Static_assert(!(sizeof(key) & 3), "");
	hash32 hash = hash_murmurhash1((u32 *) &key, sizeof(key) >> 2, 0);
	ASSERT(hash != 0);

	F_GlyphEntry *sparse = fc->table->sparse;
	u32 sparse_size = fc->table->sparse_size;

	u32 mask = sparse_size - 1;
	ASSERT(ISPOW2(sparse_size));

	u32 tail, miss;

	for (miss = 0; miss < sparse_size; ++ miss) {
		tail = (hash + miss) & mask;
		if (do_glyph_keys_match(sparse[tail].key, key)) {
			status = STATUS_PRESENT;
			break;
		}
		else if (sparse[tail].key.rune == 0) {
			sparse[tail].key = key;
			break;
		}
	}
	F_GlyphQuery query;
	query.status = status;
	query.cache = & sparse[tail].cache;
	return query;
}

F_Cache *F_NewFontCache() {
	Stack stack = stack_new(0, "font cache stack");

	u32 sparse_size = 4096;

	F_GlyphTable *table = stack_push_zero(stack, sizeof(*table) + sizeof(*table->sparse) * sparse_size);
	table->sparse_size = sparse_size;

	F_Cache *cache = stack_push_zero(stack, sizeof(*cache));
	cache->stack = stack;
	cache->table = table;
	cache->supersampling = 1.f;
	return cache;
}

#define MASK 15
#define ALIGN(x) (((x) + (MASK)) & ~(MASK))

typedef struct {
	F_GlyphPage  *page;
	Image_r_u8    dest;
	vec2i         pos;
} Glyph_Space;

FILEFUNC
Glyph_Space F_AllocGlyphSpace(F_Cache *fc, vec2i size) {
	ASSERT(size.x > 0);
	ASSERT(size.y > 0);

	// -- Find any free page
	F_GlyphPage *page = 0;
	F_GlyphPage *next;
	for (next = fc->pages; next < fc->pages + fc->npages; ++ next) {
		// -- Can it fit in the current row?
		if (next->alloc_pos.x + size.x <= next->atlas.reso.x && next->alloc_pos.y + size.y <= next->atlas.reso.y) {
			page = next;
			break;
		}
		// -- Can it fit in the next row?
		if (next->alloc_pos.y + next->alloc_row + size.y <= next->atlas.reso.y) {
			page = next;
			break;
		}
	}

	if (!page) {
		ASSERT(fc->npages < MAX_FONT_PAGES);
		page = & fc->pages[fc->npages ++];
		clear_memory(page, sizeof(*page));
		// Todo, what should be the page size?
		vec2i reso = { 1024, 1024 };
		// -- Allocate atlas image buffer, already cleared...
		page->atlas = push_image_r_u8(fc->stack, reso);
	}

	// -- Can we allocate within the current row? No? Create a new row...
	if (page->alloc_pos.x + size.x > page->atlas.reso.x) {
		page->alloc_pos.y += page->alloc_row;
		page->alloc_pos.x = 0;
		page->alloc_row = 0;
	}
	// -- Ensure glyph fits within current row (should never fail)
	ASSERT(page->alloc_pos.y + size.y <= page->atlas.reso.y);

	// -- Resulting glyph position
	vec2i pos = page->alloc_pos;

	// -- Advance row cursor
	page->alloc_pos.x += size.x;

	// -- Stretch row to fit glyph
	page->alloc_row = Max(page->alloc_row, size.y);

	// -- Resulting image slice for rasterizer
	Image_r_u8 dest = slice_image_r_u8(page->atlas, rect_i32_from_pos_size(pos, size));

	Glyph_Space alloc = {
		.page = page,
		.dest = dest,
		.pos  = pos,
	};
	return alloc;
}

FILEFUNC
f32 ttf_em_to_pixels_factor(TTF_Font *ttf, f32 height) {
	f32 result = (f32) height / (f32) (ttf->ascent - ttf->descent);
	return result;
}

FILEFUNC
void F_BuildCache(F_Cache *fc, F_GlyphKey key, F_GlyphCache *cache) {
	check_glyph_key(key);

	Rune      rune   = key.rune;
	i32       height = key.height;
	TTF_Font *font   = key.font;


	TTF_Glyph glyph = ttf_glyph_from_rune(font, rune);

	// -- Compute final output scale
	f32 scale = ttf_em_to_pixels_factor(font, height);
	TTF_BoundingBox glyph_box = ttf_get_glyph_bounding_box(glyph, (TTF_Transform) { v2(scale, scale) });
	vec2i glyph_size = v2i_sub(glyph_box.p1, glyph_box.p0);


	if (glyph_size.x != 0 && glyph_size.y != 0) {
		// -- Allocate space from atlas
		Glyph_Space alloc = F_AllocGlyphSpace(fc, v2i(ALIGN(glyph_size.x), ALIGN(glyph_size.y)));

		F_GlyphPage  *page      = alloc.page;
		Image_r_u8    dest      = alloc.dest;
		vec2i         glyph_pos = alloc.pos;

		TTF_Raster raster = {
			.src   = glyph,
			.dst   = dest,
			.xform.scale = v2(scale, - scale),
		};
		ttf_rasterize(font, &raster);
		++ page->dirty;

		// Todo, store this? ...
		f32 inv_w = 1.0 / page->atlas.reso.x;
		f32 inv_h = 1.0 / page->atlas.reso.y;

		cache->page   = page;
		cache->w      = glyph_size.x;
		cache->h      = glyph_size.y;
		cache->src.u0 = (glyph_pos.x +                0) * inv_w;
		cache->src.v0 = (glyph_pos.y +                0) * inv_h;
		cache->src.u1 = (glyph_pos.x + glyph_size.x) * inv_w;
		cache->src.v1 = (glyph_pos.y + glyph_size.y) * inv_h;
	}

	cache->x_advance = glyph.x_advance * scale;
	cache->x_bearing = glyph_box.x0;
	cache->y_bearing = glyph_box.y0;
}

FILEFUNC
F_GlyphCache *F_FetchGlyphFromKey(F_Cache *fc, F_GlyphKey key) {
	check_glyph_key(key);
	F_GlyphQuery query = F_QueryGlyphCache(fc, key);
	// Todo, we're not interested in whether the entry is in the
	// table or not, we're interested in whether the glyph is cached or not...
	if (query.status == STATUS_MISSING) {
		F_BuildCache(fc, key, query.cache);
	}
	return query.cache;
}

// void f_preload_ascii_range(Font_Aspects aspects) {
// 	// preload ascii range...
// 	for (int i = 32; i < 127; ++i) {
// 		F_GlyphKey key = { .rune = i, .aspects = aspects };
// 		F_FetchGlyphFromKey(key);
// 	}
// }

typedef struct {
	Rune rune;
	u32  size;
} RuneWithSize;

typedef struct {
	u32  num_runes;
	Rune     runes[];
} RuneArray;

// Todo, safety
FILEFUNC
RuneWithSize rune_with_size_from_utf8(const u8 *c)
{
	Rune r = 0, s = 0;
	if (c[0] < 0x80) s = 1, r = (c[0] & 0xFF);
	else if (c[0] < 0xDF) s = 2, r = (c[0] & 0x1F) <<  6 | (c[1] & 0x3F) << 0;
	else if (c[0] < 0xEF) s = 3, r = (c[0] & 0x0F) << 12 | (c[1] & 0x3F) <<  6 | (c[2] & 0x3F) << 0;
	else if (c[0] < 0xF7) s = 4, r = (c[0] & 0x07) << 18 | (c[1] & 0x3F) << 12 | (c[2] & 0x3F) << 6 | (c[3] & 0x3F) << 0;
	RuneWithSize rws;
	rws.rune = r;
	rws.size = s;
	return rws;
}

FILEFUNC
RuneArray *rune_array_from_string(Stack stack, String string)
{
	stack_align(stack, sizeof(Rune));
	RuneArray *result = stack_push_zero(stack, sizeof(*result));
	RuneWithSize r;
	for (u32 i = 0; i < string.size; i += r.size) {
		r = rune_with_size_from_utf8((u8 *) string.data + i);
		if (r.size == 0) break;
		stack_push_copy(stack, sizeof(r.rune), & r.rune);
		++ result->num_runes;
	}
	return result;
}

// Todo, in this approach:
// Convert text to runes
// Fetch cached glyphs for each rune into a cache array, each cache entry points to the page it belongs,
// increment its ref counter (reset per run).
// Now we know how many passes and the length of each.
// Allocate passes and bind each pass to its page (each page has a pass pointer).
// Layout each rune and write the resulting quad to the glyph -> page -> pass
//
//	Todo, other option.
// Each page could instead have its own per run arena.
// Convert to runes, for each rune, query glyph, layout the glyph, add the glyph to its pass
// arena.
// In the end, you have all the pages pointing to a buffer of quads.
// For each page, copy the all the passes into the resulting stack.
//
FILEFUNC
F_Run F_BuildTextRun(F_Cache *fc, Stack stack, TTF_Font *font, i32 height, String string) {
	// why would you even call
	ASSERT(string.text);
	ASSERT(string.size);

	F_Run result;


	TIME_FRAME(__func__) {
		for (int i = 0; i < fc->npages; ++ i) {
			fc->pages[i].pass   = 0;
			fc->pages[i].nquads = 0;
		}

		// -- String -> Rune Array
		RuneArray *rune_array = rune_array_from_string(stack, string);

		// -- Rune Array -> Cache Array
		F_GlyphCache *cache_array = stack_push_zero(stack, sizeof(* cache_array) * rune_array->num_runes);

		for (u32 i = 0; i < rune_array->num_runes; ++ i) {
			Rune rune = rune_array->runes[i];

			F_GlyphCache *cache = F_FetchGlyphFromKey(fc, make_glyph_key(rune, font, height));
			if (cache) {
				cache_array[i] = *cache;
				// -- increment num refs == num quads per pass
				if (cache->page) cache->page->nquads ++;
			}
		}

		// -- Build passes
		F_Pass *passes;
		{
			F_Pass  pass_head = {};
			F_Pass *pass_prev = & pass_head;

			F_GlyphPage *page;
			for (page = fc->pages; page < fc->pages + fc->npages; ++ page) {
				// -- Is the paged marked?
				if (page->nquads) {
					// -- Allocate pass
					// Todo, would it be worth to just emit all the quads and passes are just offsets into the
					// quad array ? We'd only really save a pointer ...
					// But if we integrate better with the renderer, it would be one single mem copy to the vertex
					// buffer... So it might be worth doing to facilitate this in the future...
					F_Pass *pass = stack_push(stack, sizeof(*pass) + sizeof(*pass->quads) * page->nquads);
					pass->page    = page;
					pass->nquads  = 0;
					pass->prox    = 0;
					page->pass    = pass;
					pass_prev = pass_prev->prox = pass;
				}
			}
			passes = pass_head.prox;
		}

		f32 scale = height / (f32) (font->ascent - font->descent);

		f32 row_size_em = font->ascent - font->descent + font->line_gap;
		f32 row_size = ceilf(row_size_em * scale);


		f32 x, y = 0;
		u32 i = 0;
		f32 w = 0.f;

		Rune *cur_rune = rune_array->runes;
		Rune *end_rune = rune_array->runes + rune_array->num_runes;

		while (cur_rune < end_rune)
		{

			for (x = 0.f; cur_rune < end_rune && * cur_rune != '\n' && * cur_rune != '\r'; ++ cur_rune)
			{
				Rune rune = *cur_rune;
				u32 rune_index = cur_rune - rune_array->runes;
				F_GlyphCache cache = cache_array[rune_index];

				if (cache.page != 0 && rune != ' ' && rune != '\t' && rune != '\v')
				{
					F_Quad *quad = cache.page->pass->quads + cache.page->pass->nquads ++;
					quad->src = cache.src;
					quad->dst.x = x + cache.x_bearing;
					quad->dst.y = y + cache.y_bearing;
					quad->dst.w = cache.w;
					quad->dst.h = cache.h;
					quad->dst.x = floorf(quad->dst.x);
					quad->dst.y = floorf(quad->dst.y);
				}

				x += cache.x_advance;
			}

			y += row_size;
			w = Max(w, x);

			// -- If this condition is true, we must be at a '\r' or '\n'
			// Handles both line endings, '\n' and '\r\n'
			if (cur_rune < end_rune && *cur_rune == '\r') {
				++ cur_rune;
			}
			if (cur_rune < end_rune && *cur_rune == '\n') {
				++ cur_rune;
			}
		}

		result.passes = passes;
		result.dim.x = w;
		result.dim.y = y;
	}
	return result;
}
