
// ELF_FUNCTION(L_NewTileSet) {
// 	vec2i reso;
// 	_load_vec2i(S, 1, nargs, &reso);

// 	i32 nregions = elf_loadint(S, 3);

// 	UV_Map *rm = AttachUVs(reso, nregions);

// 	// todo: integer as pointer
// 	elf_pushint(S, (elf_Integer) rm);
// 	return 1;
// }

// ELF_FUNCTION(L_SetTileSet) {
// 	// todo: integers as pointers
// 	g_core.tileset = (UV_Map *) elf_loadint(S, 1);
// 	return 0;
// }

// ELF_FUNCTION(L_GetTileCoords) {
// 	i32 id = elf_loadint(S, 1);

// 	checkindex(S, id, g_core.tileset->nregions);

// 	UV_Coords coords = GetRegionCoords(g_core.tileset, id);
// 	elf_pushnum(S, coords.u0);
// 	elf_pushnum(S, coords.v0);
// 	elf_pushnum(S, coords.u1);
// 	elf_pushnum(S, coords.v1);
// 	return 4;
// }

// ELF_FUNCTION(L_SetTileCoords) {

// 	i32 id = elf_loadint(S, 1);
// 	checkindex(S, id, g_core.tileset->nregions);

// 	i32 x = elf_loadint(S, 2);
// 	i32 y = elf_loadint(S, 3);
// 	i32 w = elf_loadint(S, 4);
// 	i32 h = elf_loadint(S, 5);


// 	SetRegionCoords(g_core.tileset, id, x, y, w, h);
// 	return 0;
// }

// elf_Binding l_region_mapper[] = {
// 	{"NewTileSet"         , L_NewTileSet         },
// 	{"SetTileSet"         , L_SetTileSet         },
// 	{"SetTileCoords"      , L_SetTileCoords      },
// 	{"GetTileCoords"      , L_GetTileCoords      },
// };
