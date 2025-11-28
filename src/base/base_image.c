
Image_r_u8 image_r_u8_from_data(vec2i reso, u32 elem_stride, void *data) {
	Image_r_u8 image = {
		.reso        = reso,
		.data        = data,
		.elem_stride = elem_stride,
	};
	return image;
}

Image_rgba_u8 image_rgba_u8_from_data(vec2i reso, u32 elem_stride, void *data) {
	Image_rgba_u8 image = {
		.reso        = reso,
		.data        = data,
		.elem_stride = elem_stride,
	};
	return image;
}

void *push_generic_image(Stack arena, vec2i reso, u32 elem_size, u32 fill) {
	return stack_push_fill(arena, elem_size * reso.x * reso.y, fill);
}


Image_rgba_u8 push_image_rgba_u8_filled(Stack arena, vec2i reso, u32 fill) {
	Image_rgba_u8 image;
	void *data = push_generic_image(arena, reso, sizeof(* image.data), fill);
	image = image_rgba_u8_from_data(reso, reso.x, data);
	return image;
}

Image_r_u8 push_image_r_u8_filled(Stack arena, vec2i reso, u32 fill) {
	Image_r_u8 image;
	void *data = push_generic_image(arena, reso, sizeof(* image.data), fill);
	image = image_r_u8_from_data(reso, reso.x, data);
	return image;
}

Image_r_u8 push_image_r_u8(Stack arena, vec2i reso) {
	return push_image_r_u8_filled(arena, reso, 0);
}

Image_rgba_u8 push_image_rgba_u8(Stack arena, vec2i reso) {
	return push_image_rgba_u8_filled(arena, reso, 0);
}



Image_r_u8 slice_image_r_u8(Image_r_u8 image, rect_i32 r) {
	r = rect_i32_clip(rect_i32_from_size(image.reso), r);
	Image_r_u8 slice = image_r_u8_from_data(r.size, image.elem_stride, image.data + image.elem_stride * r.y + r.x);
	return slice;
}

Image_rgba_u8 slice_image_rgba_u8(Image_rgba_u8 image, rect_i32 r) {
	r = rect_i32_clip(rect_i32_from_size(image.reso), r);
	Image_rgba_u8 slice = image_rgba_u8_from_data(r.size, image.elem_stride, image.data + image.elem_stride * r.y + r.x);
	return slice;
}

