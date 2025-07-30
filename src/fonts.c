
// <3
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "elements.h"
#include "platform.h"
#include "renderer.h"
#include "draw_2d.h"


enum {
	FONTS_CAPACITY       = 16,
	FONT_NAME_CAPACITY   = 128,
	FONT_GLYPHS_CAPACITY = 256,
};

typedef struct D_FONT D_FONT;
struct D_FONT {
	TextureId       texture;
	char              name[FONT_NAME_CAPACITY];
	stbtt_bakedchar glyphs[FONT_GLYPHS_CAPACITY];
};

global D_FONT g_fonts[FONTS_CAPACITY];

int InstallFont(R_Renderer *rend, int id, char *name, int size) {
	// todo:
	D_FONT *font = &g_fonts[id];

	// todo:
	enum { TEMP_SIZE = MEGABYTES(8) };
	unsigned char *data = malloc(TEMP_SIZE);
	OS_ReadEntireFile(name, data, TEMP_SIZE);

	int r = 1024;

	unsigned char *atlas = malloc(r * r);
	int error = stbtt_BakeFontBitmap(data, 0, size, atlas, r, r, 32, 95, (stbtt_bakedchar *) font->glyphs);
	if (error == 0) {
		fprintf(stderr, "Could Not Create Font\n");
	}

	font->texture = R_InstallTexture(rend, R_NewTextureId(rend), FORMAT_R8_UNORM, (vec2i){r, r}, atlas);

	free(data);
	free(atlas);

	// char buffer[256];
	// {
	// 	sprintf_s(buffer, sizeof(buffer), "%s.font", output_name, buffer);
	// 	FILE *file = fopen(buffer, "wb");
	// 	if (!file) fprintf(stderr, "invalid name");
	// 	fwrite(&ff, 1, sizeof(ff), file);
	// 	fclose(file);
	// }
	// {
	// 	sprintf_s(buffer, sizeof(buffer), "%s.bmp", output_name, buffer);
	// 	stbi_write_bmp(buffer,r,r,1,atlas);
	// }

	return id;
}




//
// todo: proper text rendering...
//
void D_DrawText(R_Renderer *rend, f32 x, f32 y, char *text) {
	Color color = D_GetColor0();

	D_FONT *font = & g_fonts[D_GetFont()];

	vec2i resolution = R_GetTextureInfo(rend, font->texture);
	f32 inv_resolution_x = 1.0 / resolution.x;
	f32 inv_resolution_y = 1.0 / resolution.y;
	R_SetTopology(rend, TOPO_TRIANGLES);
	R_SetTexture(rend, font->texture);

	while (*text) {
		i32 token = *text ++;
		i32 index = token - 32;

		stbtt_bakedchar glyph = font->glyphs[index];

		r_f32 src_r = { glyph.x0, glyph.y0, glyph.x1 - glyph.x0, glyph.y1 - glyph.y0 };
		r_f32 dst_r = { x + glyph.xoff, y - (src_r.h + glyph.yoff), src_r.w, src_r.h };

		vec3 r_p0 = { dst_r.x +       0, dst_r.y +       0, 0 };
		vec3 r_p1 = { dst_r.x +       0, dst_r.y + dst_r.h, 0 };
		vec3 r_p2 = { dst_r.x + dst_r.w, dst_r.y + dst_r.h, 0 };
		vec3 r_p3 = { dst_r.x + dst_r.w, dst_r.y +       0, 0 };

		if (token != ' ') {

			f32 u0 = src_r.x * inv_resolution_x;
			f32 v0 = src_r.y * inv_resolution_y;
			f32 u1 = (src_r.x + src_r.w) * inv_resolution_x;
			f32 v1 = (src_r.y + src_r.h) * inv_resolution_y;

			R_Vertex3 *vertices = R_QueueVertices(rend, 6);
			vertices[0]=(R_Vertex3){r_p0,{u0,v1},color};
			vertices[1]=(R_Vertex3){r_p1,{u0,v0},color};
			vertices[2]=(R_Vertex3){r_p2,{u1,v0},color};
			vertices[3]=(R_Vertex3){r_p0,{u0,v1},color};
			vertices[4]=(R_Vertex3){r_p2,{u1,v0},color};
			vertices[5]=(R_Vertex3){r_p3,{u1,v1},color};
		}

		x += glyph.xadvance;
	}
}