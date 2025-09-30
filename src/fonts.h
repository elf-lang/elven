
typedef struct {
	f32 x, y, w, h;
	short x_bearing, y_bearing;
	short x_advance, user_data;
} Font_Glyph_Data;


typedef struct Loaded_Font Loaded_Font;
typedef Loaded_Font *FONT_HANDLE;


FONT_HANDLE NewFont(int num_glyphs);
void SetFontTexture(FONT_HANDLE font, RID texture);
void SetFontGlyph(FONT_HANDLE font, int rune, Font_Glyph_Data data);


FONT_HANDLE LoadTrueTypeFont(void *data, int font_size);
int UninstallFont(FONT_HANDLE font);
f32 MeasureText(FONT_HANDLE font, const char *text);
void DrawText(FONT_HANDLE font, f32 x, f32 y, const char *text);
