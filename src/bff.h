//
// baked font file (.bff)
//
//
// Usage:
//
// 	file := open_file(...)
//		size := size_file(file)
//		bff_FILE_DATA *data = alloc_memory(size)
//		read_file(file, data, size)
//
//


typedef struct {
	// glyph coordinates within the atlas
	unsigned short x, y, w, h;
	// drawing offset to properly align the glyph
	short x_bearing, y_bearing;

	short x_advance, unused;
} bff_Glyph;


typedef struct {
	char      magic_bytes[3];
	char      version;
	short     atlas_width;
	short     atlas_height;
	short     num_glyphs;
} bff_FILE_HEADER;


typedef struct {
	char          magic_bytes[3];
	char          version;
	char          format;
	int           atlas_width;
	int           atlas_height;
	bff_Glyph     glyphs[256];
	unsigned char atlas_data[];
} bff_FILE_DATA;