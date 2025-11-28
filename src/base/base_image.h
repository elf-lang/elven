
// Todo, rename elem_stride to just stride
typedef struct {
	vec2i        reso;
	u32   elem_stride;
	u8          *data;
} Image_r_u8;

typedef struct {
	vec2i              reso;
	u32         elem_stride;
	Color_rgba_u8     *data;
} Image_rgba_u8;

Image_rgba_u8 push_image_rgba_u8(Stack arena, vec2i reso);
Image_r_u8 push_image_r_u8(Stack arena, vec2i reso);
Image_r_u8 push_image_r_u8_filled(Stack arena, vec2i reso, u32 color);
Image_rgba_u8 push_image_rgba_u8_filled(Stack arena, vec2i reso, u32 fill);

Image_r_u8 image_r_u8_from_rgba_u8(Image_rgba_u8 image);

Image_rgba_u8 slice_image_rgba_u8(Image_rgba_u8 i, rect_i32 r);
Image_r_u8 slice_image_r_u8(Image_r_u8 i, rect_i32 r);

// Todo: remove these!
PORTABLEFUNC
void image_draw_rect(Image_r_u8 image, u32 color, rect_i32 r) {
	r = rect_i32_clip(rect_abs(r), rect_i32_from_size(image.reso));
	i32 x0 = r.x;
	i32 y0 = r.y;
	i32 x1 = r.x + r.w;
	i32 y1 = r.y + r.h;
	for (i32 y = y0; y < y1; ++ y) {
		for (i32 x = x0; x < x1; ++ x) {
			image.data[image.elem_stride * y + x] = color;
		}
	}
}

PORTABLEFUNC
void image_draw_rect_outline(Image_r_u8 img, u32 color, rect_i32 r) {
	image_draw_rect(img, color, (rect_i32){ r.x + 0    , r.y +     0, r.w, 1 });
	image_draw_rect(img, color, (rect_i32){ r.x + 0    , r.y + r.h-1, r.w, 1 });
	image_draw_rect(img, color, (rect_i32){ r.x + 0    , r.y +     0, 1, r.h });
	image_draw_rect(img, color, (rect_i32){ r.x + r.w-1, r.y +     0, 1, r.h });
}
