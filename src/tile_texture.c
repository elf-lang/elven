//
//	This is purely a runtime structure, you load whatever data
// you have, you transform it into this form, and then draw.
//
// It doesn't do any culling or clipping for you, think of this
// as an image that is made up of tiles instead of pixels.
//
// The idea is to have an optimized rendering path for this.
//
//	todo: Of course, there isn't an optimized rendering path yet...
//




typedef u32 Tile;



typedef struct TileTexture TileTexture;
struct TileTexture {
	// size in log2 for virtually no reason...
	int   w_log2;
	int   h_log2;

	// Other state specific to the rendering path can be
	// added here...
	// For instance, a render target to cache the last render
	// Or a vertex buffer
	int   tile_w;
	int   tile_h;
	Tile tiles[];
};


#define layermask_x(l) (LOG2MASK((l)->w_log2))
#define layermask_y(l) (LOG2MASK((l)->h_log2))
#define layerwrap_x(l,x) (layermask_x(l) & (x))
#define layerwrap_y(l,y) (layermask_y(l) & (y))
#define layerhash(l, x, y) (layerwrap_x(l, x) | layerwrap_y(l, y) << (l)->w_log2)


static inline Tile tt_read(TileTexture *tt, i32 x, i32 y) {
	return tt->tiles[layerhash(tt, x, y)];
}

static inline void tt_write(TileTexture *tt, i32 x, i32 y, Tile tile) {
	tt->tiles[layerhash(tt, x, y)] = tile;
}

static TileTexture *NewTileTexture(i32 w, i32 h, i32 tile_w, i32 tile_h) {
	ASSERT(ISPOW2(w));
	ASSERT(ISPOW2(h));

	TileTexture *tt = calloc(1, sizeof(*tt) + sizeof(tt->tiles[0]) * (w * h));
	tt->w_log2 = unshift(w);
	tt->h_log2 = unshift(h);
	tt->tile_w = tile_w;
	tt->tile_h = tile_h;
	return tt;
}

static void SetLayerTile(TileTexture *tt, int x, int y, Tile tile) {
	tt_write(tt, x, y, tile);
}

static Tile GetLayerTile(TileTexture *tt, int x, int y) {
	return tt_read(tt, x, y);
}

// I feel like naming this 'texture' gives the impression that you should
// be able to do:
//
// set_texture(tile_texture)
// draw_rectangle()
//
static void DrawTileTexture(TileTexture *tt, i32 l_x, i32 l_y, i32 l_w, i32 l_h) {

	RID texture = R_GetTexture(gd.rend);
	ASSERT(texture->uv_map);
	UV_Coords *uv_coords = texture->uv_map->coords;


	i32 w_log2 = tt->w_log2;
	i32 h_log2 = tt->h_log2;
	i32 tile_w = tt->tile_w;
	i32 tile_h = tt->tile_h;


	i32 nverts = l_w * l_h * 6;
	R_Vertex3 *verts = BeginDrawCall(TOPO_TRIANGLES, nverts);

	R_Vertex3 *cursor = verts;

	// todo: per tile coloring?
	Color color = gd.color_0;

	i32 ix, iy;
	for (iy=0; iy<l_h; ++iy) {
		for (ix=0; ix<l_w; ++ix) {
			int tile_x = (l_x + ix) & LOG2MASK(w_log2);
			int tile_y = (l_y + iy) & LOG2MASK(h_log2);
			Tile tile = tt_read(tt, tile_x, tile_y);
			// todo: *1 we can't do this because we've already reserved the verts!
			// if (!tile) continue;

			Rect dst = { ix * tile_w, iy * tile_h, tile_w, tile_h };

			UV_Coords coords = uv_coords[tile];

			vec3 r_p0 = { dst.x +     0, dst.y +     0, 0 };
			vec3 r_p1 = { dst.x +     0, dst.y + dst.h, 0 };
			vec3 r_p2 = { dst.x + dst.w, dst.y + dst.h, 0 };
			vec3 r_p3 = { dst.x + dst.w, dst.y +     0, 0 };

			f32 u0 = coords.u0;
			f32 v0 = coords.v0;
			f32 u1 = coords.u1;
			f32 v1 = coords.v1;

			cursor[0]=(R_Vertex3){r_p0,{u0,v1},color};
			cursor[1]=(R_Vertex3){r_p1,{u0,v0},color};
			cursor[2]=(R_Vertex3){r_p2,{u1,v0},color};
			cursor[3]=(R_Vertex3){r_p0,{u0,v1},color};
			cursor[4]=(R_Vertex3){r_p2,{u1,v0},color};
			cursor[5]=(R_Vertex3){r_p3,{u1,v1},color};
			cursor += 6;
		}
	}


	EndDrawCall(verts, nverts);
}


