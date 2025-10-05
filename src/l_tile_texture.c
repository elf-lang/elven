
static inline TileTexture *load_tt(elf_State *S, int x) {
	TileTexture *layer = (TileTexture *) elf_loadint(S, x);
	return layer;
}


ELF_FUNCTION(L_NewTileTexture) {
	int w = elf_loadint(S, 1);
	int h = elf_loadint(S, 2);
	int tile_w = elf_loadint(S, 3);
	int tile_h = tile_w;
	if (nargs > 4) {
		tile_h = elf_loadint(S, 4);
	}
	TileTexture *tt = NewTileTexture(w, h, tile_w, tile_h);
	tt->w_log2 = unshift(w);
	tt->h_log2 = unshift(h);
	tt->tile_w = tile_w;
	tt->tile_h = tile_h;

	// todo: integer as pointer
	elf_pushint(S, (elf_Integer) tt);
	return 1;
}

ELF_FUNCTION(L_SetLayerTile) {
	TileTexture *tt = load_tt(S, 1);
	i32 x = elf_loadint(S, 2);
	i32 y = elf_loadint(S, 3);
	Tile tile = elf_loadint(S, 4);

	SetLayerTile(tt, x, y, tile);
	return 0;
}

ELF_FUNCTION(L_GetLayerTile) {
	TileTexture *tt = load_tt(S, 1);
	i32 x = elf_loadint(S, 2);
	i32 y = elf_loadint(S, 3);

	Tile tile = GetLayerTile(tt, x, y);
	elf_pushint(S, tile);
	return 1;
}

ELF_FUNCTION(L_DrawTileLayer) {
	TileTexture *tt = load_tt(S, 1);
	i32 x = elf_loadint(S, 2);
	i32 y = elf_loadint(S, 3);
	i32 w = elf_loadint(S, 4);
	i32 h = elf_loadint(S, 5);
	DrawTileTexture(tt, x, y, w, h);
	return 0;
}

elf_Binding l_tile_texture[] = {
	{"NewTileTexture"      , L_NewTileTexture    },
	{"SetLayerTile"        , L_SetLayerTile      },
	{"GetLayerTile"        , L_GetLayerTile      },
	{"DrawTileLayer"       , L_DrawTileLayer     },
};