//
// allows the user to write a script to package
// all their files into one single archive.
//
// todo: why is this bugging!
//

typedef struct {
	u16  type;
	u16  tags;
	u32  size;
	u32  extra_0;
	u32  extra_1;
	char name[128];
} Archive_Chunk;
STATIC_ASSERT(sizeof(Archive_Chunk) == 144);


global FILE_HANDLE   g_farch;
global Archive_Chunk g_chunk;


ELF_FUNCTION(L_OpenArchive) {
	const char *name = elf_loadtext(S, 1);
	g_farch = sys_open_file(name, SYS_OPEN_READ, SYS_OPEN);
	elf_pushint(S, g_farch != ELF_HINVALID);
	return 1;
}


ELF_FUNCTION(L_BeginArchive) {
	const char *name = elf_loadtext(S, 1);
	g_farch = sys_open_file(name, SYS_OPEN_WRITE, SYS_OPEN_ALWAYS);
	elf_pushint(S, g_farch != ELF_HINVALID);
	return 1;
}


ELF_FUNCTION(L_CloseArchive) {
	sys_close_file(g_farch);
	g_farch = 0;
	return 0;
}


ELF_FUNCTION(L_GetChunkName) { elf_pushtext(S, g_chunk.name); return 1; }
ELF_FUNCTION(L_GetChunkType) { elf_pushint(S, g_chunk.type); return 1; }
ELF_FUNCTION(L_GetChunkSize) { elf_pushint(S, g_chunk.size); return 1; }


ELF_FUNCTION(L_SkipChunk) {
	elf_i64 cursor = sys_move_file_cursor(g_farch, SYS_CURRENT, 0);
	elf_i64 size = sys_size_file(g_farch);
	elf_i64 rem = size - cursor;
	elf_i64 move = MIN(rem, g_chunk.size);
	sys_move_file_cursor(g_farch, SYS_CURRENT, move);
	return 0;
}


ELF_FUNCTION(L_ReadNextChunk) {
	elf_i64 cursor = sys_move_file_cursor(g_farch, SYS_CURRENT, 0);
	elf_i64 size = sys_size_file(g_farch);
	elf_i64 rem = size - cursor;
	if (rem >= sizeof(g_chunk)) {
		int read = sys_read_file(g_farch, &g_chunk, sizeof(g_chunk));
		assert(read == sizeof(g_chunk));
		elf_pushtrue(S);
	} else {
		elf_pushfalse(S);
	}
	return 1;
}



ELF_FUNCTION(L_ArchiveFile) {
	const char *file_name = elf_loadtext(S, 1);
	const char *chunk_name = elf_loadtext(S, 2);
	int chunk_type = elf_loadint(S, 3);
	assert(strlen(chunk_name) < 128);

	FILE_HANDLE file = sys_open_file(file_name, SYS_OPEN_READ, SYS_OPEN);
	if (file != ELF_HINVALID) {
		elf_i64 size = sys_size_file(file);
		assert(size <= UINT_MAX);

		char *bread = malloc(size);
		elf_i64 zread = sys_read_file(file, bread, size);
		assert(zread == size);

		sys_close_file(file);
		Archive_Chunk chunk = {
			.type = chunk_type,
			.size = size,
		};
		strcpy(chunk.name, chunk_name);
		elf_i64 wrote;
		wrote = sys_write_file(g_farch, &chunk, sizeof(chunk));
		assert(wrote == sizeof(chunk));
		wrote = sys_write_file(g_farch, bread, size);
		assert(wrote == size);

		free(bread);
		elf_pushtrue(S);
	} else {
		elf_pushfalse(S);
	}
	return 1;
}


elf_Binding l_arch[] = {
	{"GetChunkName", L_GetChunkName},
	{"GetChunkType", L_GetChunkType},
	{"GetChunkSize", L_GetChunkSize},
	{"ReadNextChunk", L_ReadNextChunk},
	{"SkipChunk", L_SkipChunk},
	{"OpenArchive", L_OpenArchive},
	{"BeginArchive", L_BeginArchive},
	{"CloseArchive", L_CloseArchive},
	{"ArchiveFile", L_ArchiveFile},
};
