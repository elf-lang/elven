//
// Where to put this?
//
//	== LONG TERM DATA STORAGE ==
//
//
// A handle may come in different forms, but it typically boils
// down to a pointer sized integer, its interpretation is unknown
// and irrelevant you.
//
//
//	You get a handle from some other system, and only the system from
// which you got the handle knows what you're referring to.
//
// So, a handle's primary purpose it to allow for your program to
// communicate with some foreign system about a thing in particular.
//
//	A handle is typically a proxy for some memory.
//
// Could be memory on the CPU, or on the GPU.
//
// Might even be special memory, "read-only", "write-only", it depends.
//
// The point is, it's probably just memory that is being used in very
// particular ways.
//
//
//
//	Whenever you call LoadTexture(), you receive a new handle, this handle is
// only valid during the duration of your program.
//
//	So if you were to save this handle to a file and attempt to reuse it
// after your program has terminated, it will be invalid.
//
//
//	Once your program ends, all those resources are reclaimed, and your handles
// to it become invalid.
//
//	This is a problem if you ever need a way to remember what it is that you're
// talking about 3 or 4 weeks from now.
//
// A practical way to do this is to use stable id's, id's that never change and
// always refer to the same concept or idea you have in mind.
//
//
//
//	TEXTURES :: enum {
//		RID_NONE = 0,
//		TEXTURE_TILESET,
//		TEXTURE_MAIN_CHARACTER,
//		TEXTURE_BAD_GUY_0,
//		TEXTURE_BAD_GUY_1,
//	}
//
//
//
//
//	live_handles := {}
//
//	LoadTextureId :: function(id, name) ? {
//		live_handles[id] = LoadTexture(name)
//	}
//
//	SetTextureId :: function(id) ? {
//		SetTexture(live_handles[id])
//	}
//
//
//	; load the texture for this id
//	LoadTexture(TEXTURE_MAIN_CHARACTER, "data/main_char.png")
//
//	; bind the texture for this id
//	SetTextureId(TEXTURE_MAIN_CHARACTER)
//
//
//
//
//
//
// the purpose of this library is to give 2d games
// the ability to render tilemaps much more efficiently,
// be able to load and unload tilemaps and tilesets to disk
// compactly and reduce boiler-plate code overall.
//
//
//
// Usage:
//
//	texture   := LoadTexture(...)
//
//	tilemap := NewTileMap(1024, 1024)
//	tileset := NewTileSet(8, 256, ...)
//
//	SetTileMap(tilemap)
//	SetTileSet(tileset)
//
//
//	DrawTileMap(0, 0,  0, 0, 0, 44, 22, 4)
//
//


typedef struct {
	short x, y;
	int   tags;
} TileCoords;


typedef struct TileSet TileSet;
struct TileSet {
	// remember texture
	u64        resource;
	short      tile_size_log2;
	short      num_tiles;
	TileCoords coords[];
};



typedef struct TileStack TileStack;
struct TileStack {
	u8 e[4];
};


typedef struct TileMap TileMap;
struct TileMap {
	u8         w_log2;
	u8         h_log2;
	u8         layers;
	u8         unused;
	TileStack  tiles[];
};


#define ISPOW2(x) ((x) != 0 && !((x) & ((x) - 1)))
#define L2MASK(x) ((1 << (x)) - 1)



static int unshift(int x) {
	unsigned long index;
	_BitScanForward(&index, x);
	return index;
}


ELF_FUNCTION(L_NewTileSet) {
	int tile_size = elf_loadint(S, 1);
	int num_tiles = elf_loadint(S, 2);
	u64 resource = elf_loadint(S, 3);

	ASSERT(ISPOW2(tile_size));
	ASSERT(ISPOW2(num_tiles));

	TileSet *tileset = calloc(1, sizeof(*tileset) + sizeof(tileset->coords[0]) * num_tiles);
	tileset->resource = resource;
	tileset->tile_size_log2 = unshift(tile_size);
	tileset->num_tiles = num_tiles;

	// todo: integer as pointer
	elf_pushint(S, (elf_Integer) tileset);
	return 1;
}


ELF_FUNCTION(L_SaveTileSet) {
	const char *name = elf_loadtext(S, 1);
	FILE_HANDLE file = sys_open_file(name, SYS_OPEN_WRITE, SYS_OPEN_ALWAYS);
	i64 extra = sizeof(gd.tileset->coords[0]) * gd.tileset->num_tiles;
	sys_write_file(file, gd.tileset, sizeof(*gd.tileset) + extra);
	sys_close_file(file);
	return 0;
}


ELF_FUNCTION(L_LoadTileSet) {
	const char *name = elf_loadtext(S, 1);

	FILE_HANDLE file = sys_open_file(name, SYS_OPEN_READ, SYS_OPEN);

	TileSet *tileset = malloc(sys_size_file(file));
	sys_read_file(file, tileset, sys_size_file(file));

	sys_close_file(file);

	elf_pushint(S, (elf_Integer) tileset);
	return 1;
}




ELF_FUNCTION(L_SaveTileMap) {
	const char *name = elf_loadtext(S, 1);
	FILE_HANDLE file = sys_open_file(name, SYS_OPEN_WRITE, SYS_OPEN_ALWAYS);
	i64 tiles_size = sizeof(gd.tilemap->tiles[0]) << (gd.tilemap->w_log2 + gd.tilemap->h_log2);
	sys_write_file(file, gd.tilemap, sizeof(*gd.tilemap) + tiles_size);
	sys_close_file(file);
	return 0;
}


ELF_FUNCTION(L_LoadTileMap) {
	const char *name = elf_loadtext(S, 1);

	FILE_HANDLE file = sys_open_file(name, SYS_OPEN_READ, SYS_OPEN);

	TileMap *tilemap = malloc(sys_size_file(file));
	sys_read_file(file, tilemap, sys_size_file(file));

	sys_close_file(file);

	elf_pushint(S, (elf_Integer) tilemap);
	return 1;
}



ELF_FUNCTION(L_NewTileMap) {
	int w = elf_loadint(S, 1);
	int h = elf_loadint(S, 1);

	ASSERT(ISPOW2(w));
	ASSERT(ISPOW2(h));

	TileMap *tilemap = calloc(1, sizeof(*tilemap) + sizeof(tilemap->tiles[0]) * (w * h));
	tilemap->w_log2 = unshift(w);
	tilemap->h_log2 = unshift(h);

	// todo: integer as pointer
	elf_pushint(S, (elf_Integer) tilemap);
	return 1;
}



ELF_FUNCTION(L_SetTileMap) {
	// todo: integers as pointers
	gd.tilemap = (TileMap *) elf_loadint(S, 1);
	return 0;
}



ELF_FUNCTION(L_SetTileSet) {
	// todo: integers as pointers
	gd.tileset = (TileSet *) elf_loadint(S, 1);
	return 0;
}



ELF_FUNCTION(L_GetTileSetResource) {
	elf_pushint(S, gd.tileset->resource);
	return 1;
}



ELF_FUNCTION(L_SetTileCoords) {
	int tile = elf_loadint(S, 1);
	int x = elf_loadint(S, 2);
	int y = elf_loadint(S, 3);
	assert(tile < gd.tileset->num_tiles);
	gd.tileset->coords[tile].x = x;
	gd.tileset->coords[tile].y = y;
	return 0;
}



ELF_FUNCTION(L_SetTileTags) {
	int tile = elf_loadint(S, 1);
	int tags = elf_loadint(S, 2);
	assert(tile < gd.tileset->num_tiles);
	gd.tileset->coords[tile].tags = tags;
	return 0;
}



ELF_FUNCTION(L_GetTileTags) {
	int tile = elf_loadint(S, 1);
	assert(tile < gd.tileset->num_tiles);
	elf_pushint(S, gd.tileset->coords[tile].tags);
	return 1;
}



static inline int accesstile(int x, int y, int z, int write, int data) {
	int tx = x & L2MASK(gd.tilemap->w_log2);
	int ty = y & L2MASK(gd.tilemap->h_log2);
	int tz = z & 3;
	int tile = gd.tilemap->tiles[tx | ty << gd.tilemap->w_log2].e[tz];
	if (write) {
		gd.tilemap->tiles[tx | ty << gd.tilemap->w_log2].e[tz] = data;
	}
	return tile;
}



ELF_FUNCTION(L_GetMapTags) {
	int x = elf_loadint(S, 1);
	int y = elf_loadint(S, 2);
	int z = elf_loadint(S, 3);

	int tile = accesstile(x, y, z, false, 0);
	int tags = gd.tileset->coords[tile].tags;
	elf_pushint(S, tags);
	return 1;
}



ELF_FUNCTION(L_GetMapTile) {
	int x = elf_loadint(S, 1);
	int y = elf_loadint(S, 2);
	int z = elf_loadint(S, 3);

	int tile = accesstile(x, y, z, false, 0);
	elf_pushint(S, tile);
	return 1;
}



ELF_FUNCTION(L_SetMapTile) {
	int x = elf_loadint(S, 1);
	int y = elf_loadint(S, 2);
	int z = elf_loadint(S, 3);
	int tile = elf_loadint(S, 4);

	accesstile(x, y, z, true, tile);
	return 0;
}



ELF_FUNCTION(L_ToggleLayer) {
	int layer = elf_loadint(S, 1);
	gd.layers_off ^= 1 << layer;
	return 0;
}



ELF_FUNCTION(L_DrawTileMap) {
	f32 px       = elf_loadnum(S, 1);
	f32 py       = elf_loadnum(S, 2);
	int map_x    = elf_loadint(S, 3);
	int map_y    = elf_loadint(S, 4);
	int map_z    = elf_loadint(S, 5);
	int map_nx   = elf_loadint(S, 6);
	int map_ny   = elf_loadint(S, 7);
	int map_nz   = elf_loadint(S, 8);

	vec2 inv_resolution = gd.texture_inv_resolution;

	int w_log2 = gd.tilemap->w_log2;
	int h_log2 = gd.tilemap->h_log2;
	int tz_log2 = gd.tileset->tile_size_log2;

	// backend could do this more optimally if it had a special
	// shader or something
	// or maybe we can just create vertex buffers per tilemap
	int num_vertices = map_nx * map_ny * map_nz * 6;
	R_Vertex3 *vertices = R_QueueVertices(gd.rend, num_vertices);
	R_Vertex3 *cursor = vertices;


	int ix, iy, iz;
	for (iz=0; iz<map_nz; ++iz){
		int map_tile_z = map_z + iz & 3;

		Color color = gd.colors[map_tile_z];
		// todo: once we implement *1
		if (gd.layers_off & 1 << map_tile_z) {
			color.a = 0;
		}

		for (iy=0; iy<map_ny; ++iy){
			for (ix=0; ix<map_nx; ++ix){

				int map_tile_x = (map_x + ix) & L2MASK(w_log2);
				int map_tile_y = (map_y + iy) & L2MASK(h_log2);

				int tile = gd.tilemap->tiles[map_tile_x | map_tile_y << w_log2].e[map_tile_z];

				// todo: *1 we can't do this because we've already reserved the vertices!
				// if (!tile) continue;

				Rect dst = {
					ix << tz_log2,
					iy << tz_log2,
					1  << tz_log2,
					1  << tz_log2
				};
				iRect src = {
					gd.tileset->coords[tile].x,
					gd.tileset->coords[tile].y,
					1 << tz_log2,
					1 << tz_log2
				};


				vec3 r_p0 = { dst.x +     0, dst.y +     0, 0 };
				vec3 r_p1 = { dst.x +     0, dst.y + dst.h, 0 };
				vec3 r_p2 = { dst.x + dst.w, dst.y + dst.h, 0 };
				vec3 r_p3 = { dst.x + dst.w, dst.y +     0, 0 };

				f32 u0 = src.x * inv_resolution.x;
				f32 v0 = src.y * inv_resolution.y;
				f32 u1 = (src.x + src.w) * inv_resolution.x;
				f32 v1 = (src.y + src.h) * inv_resolution.y;

				cursor[0]=(R_Vertex3){r_p0,{u0,v1},color};
				cursor[1]=(R_Vertex3){r_p1,{u0,v0},color};
				cursor[2]=(R_Vertex3){r_p2,{u1,v0},color};
				cursor[3]=(R_Vertex3){r_p0,{u0,v1},color};
				cursor[4]=(R_Vertex3){r_p2,{u1,v0},color};
				cursor[5]=(R_Vertex3){r_p3,{u1,v1},color};
				cursor += 6;
			}
		}
	}


	ApplyTransform(px, py, vertices, num_vertices);
	return 0;
}


elf_Binding l_tiles[] = {
	{"NewTileSet" , L_NewTileSet    },
	{"ToggleLayer", L_ToggleLayer   },
	{"NewTileMap" , L_NewTileMap    },

	{"SaveTileMap", L_SaveTileMap},
	{"LoadTileMap", L_LoadTileMap},
	{"SaveTileSet", L_SaveTileSet},
	{"LoadTileSet", L_LoadTileSet},

	{"GetTileSetResource", L_GetTileSetResource},

	{"SetTileMap", L_SetTileMap},
	{"SetTileSet", L_SetTileSet},

	{"SetTileCoords", L_SetTileCoords },
	{"SetTileTags"  , L_SetTileTags   },
	{"GetTileTags"  , L_GetTileTags   },

	{"GetMapTile", L_GetMapTile },
	{"GetMapTags", L_GetMapTags },
	{"SetMapTile", L_SetMapTile },

	{"DrawTileMap", L_DrawTileMap},
};