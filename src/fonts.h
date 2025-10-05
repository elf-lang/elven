

typedef struct Loaded_Font Loaded_Font;
typedef Loaded_Font *FONT_HANDLE;
typedef u32 Rune;


typedef struct {
	Rune rune;
	Image *src;
	int src_x, src_y, src_w, src_h;
	short x_bearing, y_bearing;
	short x_advance, user_data;
} ADD_FONT_GLYPH;



FONT_HANDLE NewFont(int num_glyphs);
void SetFontGlyph(FONT_HANDLE font, ADD_FONT_GLYPH params);


FONT_HANDLE LoadTrueTypeFont(void *data, int font_size);
int UninstallFont(FONT_HANDLE font);
f32 MeasureText(FONT_HANDLE font, const char *text);


typedef struct {
	Rect      dst;
	UV_Coords src;
} Text_Quad;

typedef struct {
	RID       texture;
	int        nquads;
	Text_Quad   quads[];
} Text_Run;

typedef struct {
	int      nruns;
	Text_Run  runs[];
} Draw_Text_Pass;


// length is the text length and the result is the number of quads
// todo: why do we pass in x and y
Draw_Text_Pass *DrawText(FONT_HANDLE hfont, const char *text, int length);
