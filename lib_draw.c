// display {
// 	image
// 	rendering_params
// }
// window {
// 	display
// 	window_params
// 	window_data
// }
// info := elf.gfx.get_rendering_backend_info()
// window := elf.gfx.create_window(320,240,"my little game",144)
// window:set_opt(window,"fullscreen",true)
// window:poll(window)
// display_0 := elf.gfx.get_window_display(window)
// display_1 := elf.gfx.create_display(320,240)
// image_0 := elf.gfx.get_display_image(display_0)
// elf.gfx.draw_rect(display_0,(0,0,8,8),(0,0,255,255))
// elf.gfx.flush_display(display_0)


// So apparently the overhead of filling out
// tables to do this is so great that this isn't
// even worth it...
// elf.video.set_color()
// elf.video.set_additive()
// elf.video.set_rect_pos()
// elf.video.set_rect_size()
// elf.video.set_rect()
// elf.video.set_image()
// elf.video.set_rotation()
// elf.video.draw()
// todo: to be made better!


static kit_Color _cur_color;
static kit_Color _cur_add_color;
static kit_Image *_cur_src_image;
static kit_Rect _cur_src_rect;
static f32x2 _cur_offset;
static f32x2 _cur_scale;
static f32x2 _cur_center;
static float _cur_rotation;

static kit_Color _tab_to_color(elf_State *S, elf_Table *tab) {
	kit_Color color = {};
	elf_Value v;
	v=elf_table_get(tab,VSTR(elf_alloc_string(S,"r")));
	color.r=v.x_int;
	v=elf_table_get(tab,VSTR(elf_alloc_string(S,"g")));
	color.g=v.x_int;
	v=elf_table_get(tab,VSTR(elf_alloc_string(S,"b")));
	color.b=v.x_int;
	v=elf_table_get(tab,VSTR(elf_alloc_string(S,"a")));
	color.a=v.x_int;
	return color;
}

static kit_Rect _tab_to_rect(elf_State *S, elf_Table *tab) {
	kit_Rect rect = {};
	elf_Value v;
	v=elf_table_get(tab,VSTR(elf_alloc_string(S,"x")));
	rect.x=VI2N(v);
	v=elf_table_get(tab,VSTR(elf_alloc_string(S,"y")));
	rect.y=VI2N(v);
	v=elf_table_get(tab,VSTR(elf_alloc_string(S,"w")));
	rect.w=VI2N(v);
	v=elf_table_get(tab,VSTR(elf_alloc_string(S,"h")));
	rect.h=VI2N(v);
	return rect;
}


static kit_Context *get_ctx(elf_State *S){
	elf_Table *_this = (elf_Table *) elf_get_this(S);
	kit_Context *ctx = (kit_Context *) elf_table_get(_this
	, VSTR(elf_push_new_string(S,"@ptr"))).x_int;
	ASSERT(ctx != 0);
	return ctx;
}

int lib_gfx_draw_rect(elf_State *S);
int lib_gfx_draw_text(elf_State *S);
int lib_draw_image(elf_State *S);
int lib_draw_circle(elf_State *S);
int lib_gfx_draw_line(elf_State *S);
int lib_gfx_draw_point(elf_State *S);

int lib_gfx_set_color(elf_State *S);
int lib_gfx_set_add_color(elf_State *S);
int lib_gfx_set_src_image(elf_State *S);
int lib_gfx_set_src_rect(elf_State *S);
int lib_gfx_set_offset(elf_State *S);
int lib_gfx_set_scale(elf_State *S);
int lib_gfx_set_rotation(elf_State *S);
int lib_gfx_set_center(elf_State *S);

void lib_gfx_draw_apply(elf_State *S, elf_Table *meta){
	elf_table_set(meta,VSTR(elf_push_new_string(S,"set_offset")),VCFN(lib_gfx_set_offset));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"set_scale")),VCFN(lib_gfx_set_scale));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"set_color")),VCFN(lib_gfx_set_color));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"set_add_color")),VCFN(lib_gfx_set_add_color));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"set_src_image")),VCFN(lib_gfx_set_src_image));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"set_src_rect")),VCFN(lib_gfx_set_src_rect));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"set_rotation")),VCFN(lib_gfx_set_rotation));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"set_center")),VCFN(lib_gfx_set_center));

	_cur_scale.x = 1;
	_cur_scale.y = 1;

	elf_table_set(meta,VSTR(elf_push_new_string(S,"draw_rect")),VCFN(lib_gfx_draw_rect));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"draw_text")),VCFN(lib_gfx_draw_text));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"draw_image")),VCFN(lib_draw_image));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"draw_circle")),VCFN(lib_draw_circle));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"draw_line")),VCFN(lib_gfx_draw_line));
	elf_table_set(meta,VSTR(elf_push_new_string(S,"draw_point")),VCFN(lib_gfx_draw_point));
}



int _get_rect(elf_State *S, int i, kit_Rect *v) {
	v->x = elf_get_int(S,i++);
	v->y = elf_get_int(S,i++);
	v->w = elf_get_int(S,i++);
	v->h = elf_get_int(S,i++);
	return i;
}

int _get_rect_trans(elf_State *S, int i, kit_Rect *v) {
	v->x = elf_get_int(S,i++) * _cur_scale.x + _cur_offset.x;
	v->y = elf_get_int(S,i++) * _cur_scale.y + _cur_offset.y;
	v->w = elf_get_int(S,i++) * fabsf(_cur_scale.x);
	v->h = elf_get_int(S,i++) * fabsf(_cur_scale.y);
	return i;
}
int _get_color(elf_State *S, int i, kit_Color *v) {
	v->r = elf_get_int(S,i++);
	v->g = elf_get_int(S,i++);
	v->b = elf_get_int(S,i++);
	v->a = elf_get_int(S,i++);
	return i;
}

int lib_gfx_set_color(elf_State *S) {
	// kit_Context *ctx = get_ctx(S);
	_get_color(S,0,&_cur_color);
	return 0;
}

int lib_gfx_set_add_color(elf_State *S) {
	// kit_Context *ctx = get_ctx(S);
	_get_color(S,0,&_cur_add_color);
	return 0;
}

int lib_gfx_set_src_image(elf_State *S) {
	// kit_Context *ctx = get_ctx(S);
	elf_Table *tab = elf_get_table(S,0);
	_cur_src_image = (kit_Image *) elf_tgets_int(tab,elf_push_new_string(S,"@ptr"));
	_cur_src_rect.x = 0;
	_cur_src_rect.y = 0;
	_cur_src_rect.w = _cur_src_image->w;
	_cur_src_rect.h = _cur_src_image->h;
	return 0;
}

int lib_gfx_set_offset(elf_State *S) {
	// kit_Context *ctx = get_ctx(S);
	_cur_offset.x = elf_get_num(S,0);
	_cur_offset.y = elf_get_num(S,1);
	return 0;
}

int lib_gfx_set_scale(elf_State *S) {
	// kit_Context *ctx = get_ctx(S);
	_cur_scale.x = elf_get_num(S,0);
	_cur_scale.y = elf_get_num(S,1);
	return 0;
}

int lib_gfx_set_src_rect(elf_State *S) {
	// kit_Context *ctx = get_ctx(S);
	_get_rect(S,0,&_cur_src_rect);
	return 0;
}

int lib_gfx_set_rotation(elf_State *S) {
	// kit_Context *ctx = get_ctx(S);
	_cur_rotation = elf_get_num(S,0);
	return 0;
}

int lib_gfx_set_center(elf_State *S) {
	// kit_Context *ctx = get_ctx(S);
	_cur_center.x = elf_get_num(S,0);
	_cur_center.y = elf_get_num(S,1);
	return 0;
}

int lib_gfx_draw_rect(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	kit_Rect rect;
	_get_rect_trans(S,0,&rect);
	kit_draw_rect(ctx,_cur_color,rect);
	return 0;
}

int lib_draw_circle(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	kit_Image *dst = ctx->screen;
	int x = elf_get_int(S,0) * _cur_scale.x + _cur_offset.x;
	int y = elf_get_int(S,1) * _cur_scale.y + _cur_offset.y;
	int r = elf_get_int(S,2) * _cur_scale.x;
	_circle(dst,x,y,r,_cur_color);
	return 0;
}

int lib_draw_image(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	kit_Image *src_i;
	kit_Color color_to_add,color_to_mul;
	color_to_add=color_to_mul=KIT_WHITE;
	int flip_x=0,flip_y=0;

	kit_Rect src_r = _cur_src_rect;
	kit_Rect dst_r = {};
	f32x2 center = _cur_center;

	int i = 0;
	int nargs = elf_get_num_args(S);
	src_i 		 = _cur_src_image;
	color_to_mul = _cur_color;
	color_to_add = _cur_add_color;
	if (nargs >= 2) { nargs -= 2;
		dst_r.x = elf_get_num(S,i++) * _cur_scale.x + _cur_offset.x;
		dst_r.y = elf_get_num(S,i++) * _cur_scale.y + _cur_offset.y;
		if (nargs >= 2) { nargs -= 2;
			dst_r.w = elf_get_num(S,i++) * fabsf(_cur_scale.x);
			dst_r.h = elf_get_num(S,i++) * fabsf(_cur_scale.y);
		} else {
			dst_r.w = _cur_src_rect.w * fabsf(_cur_scale.x);
			dst_r.h = _cur_src_rect.h * fabsf(_cur_scale.y);
		}
		// todo: it's a little weird
		if(_cur_scale.y < 0) {
			center.y += dst_r.h;
		}
	} else goto _nop;
	if (nargs >= 2) { nargs -= 2;
		flip_x = elf_get_int(S,i++);
		flip_y = elf_get_int(S,i++);
	} else goto _draw;

	_draw:

	_rect_params params = {};
	params.dst = ctx->screen;
	params.src = src_i;
	params.dst_r = dst_r;
	params.src_r = src_r;
	params.center = center; // center;
	params.angle = _cur_rotation;
	params.flip_x = flip_x;
	params.flip_y = flip_y;
	_rect(&params);

	goto _nop;
	kit_draw_image3(ctx,color_to_mul,color_to_add,src_i,dst_r,src_r);
	_nop:
	return 0;
}


int lib_gfx_draw_line(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	int i = 0;
	int x1 = elf_get_int(S,i++) * _cur_scale.x + _cur_offset.x;
	int y1 = elf_get_int(S,i++) * _cur_scale.y + _cur_offset.y;
	int x2 = elf_get_int(S,i++) * _cur_scale.x + _cur_offset.x;
	int y2 = elf_get_int(S,i++) * _cur_scale.y + _cur_offset.y;
	kit_draw_line(ctx,_cur_color,x1,y1,x2,y2);
	return 0;
}


int lib_gfx_draw_point(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	int i = 0;

	int r = elf_get_int(S,i++);
	int g = elf_get_int(S,i++);
	int b = elf_get_int(S,i++);
	int a = elf_get_int(S,i++);

	int x1 = elf_get_int(S,i++) * _cur_scale.x + _cur_offset.x;;
	int y1 = elf_get_int(S,i++) * _cur_scale.y + _cur_offset.y;;

	kit_draw_point(ctx, (kit_Color){b,g,r,a},x1,y1);
	return 0;
}

int lib_gfx_draw_text(elf_State *S) {
	kit_Context *ctx = get_ctx(S);

	int i = 0;
	elf_String *string = elf_get_string(S,i++);
	int x = elf_get_int(S,i++) * _cur_scale.x + _cur_offset.x;;
	int y = elf_get_int(S,i++) * _cur_scale.y + _cur_offset.y;;
	int r = elf_get_int(S,i++);
	int g = elf_get_int(S,i++);
	int b = elf_get_int(S,i++);
	int a = elf_get_int(S,i++);
	kit_draw_text(ctx,(kit_Color){b,g,r,a},string->text,x,y);
	return 0;
}

