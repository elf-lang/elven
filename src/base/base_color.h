
#define COLOR_TRANSPARENT  ((Color){ 0.0 * 255.f, 0.0 * 255.f, 0.0 * 255.f, 0.0 * 255.f })
#define BLACK              ((Color){ 0.0 * 255.f, 0.0 * 255.f, 0.0 * 255.f, 1.0 * 255.f })
#define WHITE              ((Color){ 1.0 * 255.f, 1.0 * 255.f, 1.0 * 255.f, 1.0 * 255.f })
#define RED                ((Color){ 1.0 * 255.f, 0.0 * 255.f, 0.0 * 255.f, 1.0 * 255.f })
#define GREEN              ((Color){ 0.0 * 255.f, 1.0 * 255.f, 0.0 * 255.f, 1.0 * 255.f })
#define BLUE               ((Color){ 0.0 * 255.f, 0.0 * 255.f, 1.0 * 255.f, 1.0 * 255.f })
#define GRAY               ((Color){ 0.5 * 255.f, 0.5 * 255.f, 0.5 * 255.f, 1.0 * 255.f })
#define DARKGRAY           ((Color){ 0.2 * 255.f, 0.2 * 255.f, 0.2 * 255.f, 1.0 * 255.f })


PORTABLEFUNC
vec4 rbga_from_packed_rgb_u32_unorm(u32 color) {
	vec4 result = { (u8)(color >> 16) / 255.f, (u8)(color >> 8) / 255.f, (u8)(color >> 0) / 255.f, 1.f };
	return result;
}

PORTABLEFUNC
vec4 rgba_from_rgb_a(vec4 color, f32 value) {
	color.a = value;
	return color;
}

PORTABLEFUNC
vec4 rgba_from_hsv(f32 hue, f32 saturation, f32 value) {
	vec4 color = { 0, 0, 0, 1.f };
	f32 k;

	k = fmodf((5.0f + hue/60.0f), 6);
	k = CLAMP(Min(4.0f - k, k), 0, 1);
	color.r = value - value*saturation*k;

	k = fmodf((3.0f + hue/60.0f), 6);
	k = CLAMP(Min(4.0f - k, k), 0, 1);
	color.g = value - value*saturation*k;

	k = fmodf((1.0f + hue/60.0f), 6);
	k = CLAMP(Min(4.0f - k, k), 0, 1);
	color.b = value - value*saturation*k;
	return color;
}
