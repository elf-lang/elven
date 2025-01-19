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
typedef struct Draw_Instr {
	kit_Color 	   color;
	kit_Color 	additive;
	kit_Rect 		 rect;
	kit_Image     *image;
	float 		rotation;
	float  outline_width;
} Draw_Instr;


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
	, VSTR(elf_new_string(S,"@ptr"))).x_int;
	ASSERT(ctx != 0);
	return ctx;
}

static int lib_video_draw(elf_State *S) {

	kit_Context *ctx = get_ctx(S);

	elf_Value _color = VSTR(elf_new_string(S,"color"));
	elf_Value _rect = VSTR(elf_new_string(S,"rect"));
	elf_Value _add = VSTR(elf_new_string(S,"additive"));

	elf_Table *draws = elf_get_table(S,0);

	kit_Image *cur_image = 0;

	kit_Color cur_color = KIT_WHITE;
	elf_Table *cur_color_ref = 0;

	kit_Color cur_add_color = KIT_BLACK;
	elf_Table *cur_add_color_ref = 0;

	elf_Value v;
	for (int i = 0; i < ARRAY_LENGTH(draws->array); i++) {
		ASSERT(draws->array[i].tag == elf_tag_tab);
		elf_Table *instr = draws->array[i].x_tab;

		v = elf_table_get(instr,_rect);
		ASSERT(v.tag == elf_tag_tab);
		kit_Rect rect = _tab_to_rect(S,v.x_tab);

		v = elf_table_get(instr,_color);
		if(v.tag != elf_tag_nil) {
			ASSERT(v.tag == elf_tag_tab);
			if (v.x_tab != cur_color_ref) {
				cur_color_ref = v.x_tab;
				cur_color = _tab_to_color(S,v.x_tab);
			}
		}

		v = elf_table_get(instr,_color);
		if(v.tag != elf_tag_nil) {
			ASSERT(v.tag == elf_tag_tab);
			if (v.x_tab != cur_color_ref) {
				cur_color_ref = v.x_tab;
				cur_color = _tab_to_color(S,v.x_tab);
			}
		}


		kit_draw_rect(ctx,cur_color,rect);

		#if 0
		v = elf_table_get(instr,_add);
		if(v.tag != elf_tag_nil) {
			ASSERT(v.tag == elf_tag_tab);
			if (v.x_tab != cur_add_color_ref) {
				cur_add_color_ref = v.x_tab;
				cur_add_color = _tab_to_color(S,v.x_tab);
			}
		}
		#endif
	}

	return 0;
}





int lib_gfx_draw_rect(elf_State *S);
int lib_gfx_draw_text(elf_State *S);
int lib_gfx_draw_image(elf_State *S);
int lib_gfx_draw_rect2(elf_State *S);
int lib_gfx_draw_line(elf_State *S);
int lib_gfx_draw_point(elf_State *S);

void lib_gfx_draw_apply(elf_State *S, elf_Table *meta){
	elf_table_set(meta
	,	VSTR(elf_new_string(S,"draw"))
	,	VCFN(lib_video_draw));

	elf_table_set(meta
	,	VSTR(elf_new_string(S,"draw_rect"))
	,	VCFN(lib_gfx_draw_rect));

	elf_table_set(meta
	,	VSTR(elf_new_string(S,"draw_text"))
	,	VCFN(lib_gfx_draw_text));

	elf_table_set(meta
	,	VSTR(elf_new_string(S,"draw_image"))
	,	VCFN(lib_gfx_draw_image));

	elf_table_set(meta
	,	VSTR(elf_new_string(S,"draw_rect2"))
	,	VCFN(lib_gfx_draw_rect2));


	elf_table_set(meta
	,	VSTR(elf_new_string(S,"draw_line"))
	,	VCFN(lib_gfx_draw_line));

	elf_table_set(meta
	,	VSTR(elf_new_string(S,"draw_point"))
	,	VCFN(lib_gfx_draw_point));
}

int lib_gfx_draw_rect(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	int i = 0;
	int x = elf_get_int(S,i++);
	int y = elf_get_int(S,i++);
	int w = elf_get_int(S,i++);
	int h = elf_get_int(S,i++);
	int r = elf_get_int(S,i++);
	int g = elf_get_int(S,i++);
	int b = elf_get_int(S,i++);
	int a = elf_get_int(S,i++);
	kit_draw_rect(ctx,(kit_Color){b,g,r,a},(kit_Rect){x,y,w,h});
	return 0;
}


int lib_gfx_draw_text(elf_State *S) {
	kit_Context *ctx = get_ctx(S);

	int i = 0;
	elf_String *string = elf_get_string(S,i++);
	int x = elf_get_int(S,i++);
	int y = elf_get_int(S,i++);
	int r = elf_get_int(S,i++);
	int g = elf_get_int(S,i++);
	int b = elf_get_int(S,i++);
	int a = elf_get_int(S,i++);
	kit_draw_text(ctx,(kit_Color){b,g,r,a},string->text,x,y);
	return 0;
}

typedef struct transform2d {
	float mul_x,mul_y;
	float add_x,add_y;
} transform2d;

int _get_rect(elf_State *S, int i, kit_Rect *v) {
	v->x = elf_get_int(S,i++);
	v->y = elf_get_int(S,i++);
	v->w = elf_get_int(S,i++);
	v->h = elf_get_int(S,i++);
	return i;
}
int _get_color(elf_State *S, int i, kit_Color *v) {
	v->r = elf_get_int(S,i++);
	v->g = elf_get_int(S,i++);
	v->b = elf_get_int(S,i++);
	v->a = elf_get_int(S,i++);
	return i;
}

typedef struct {float x,y;} f32x2;
typedef struct {float x,y;} i32x2;

typedef struct draw_params {
	kit_Image *dst;
	kit_Image *src;
	kit_Rect src_r;
	kit_Rect dst_r;
	f32x2  basis_y;
} draw_params;
static void _rect(draw_params *p) {
	kit_Image *dst = p->dst;
	kit_Rect dst_r = p->dst_r;

	float u_x = p->basis_y.x, u_y = p->basis_y.y;
	float r_x,r_y;
	r_x =   u_y;
	r_y = - u_x;
	for (int iy = 0; iy < dst_r.h; iy ++) {
		for (int ix = 0; ix < dst_r.w; ix ++) {
			int x = (int)(ix * r_x) + (int)(iy * r_y);
			int y = (int)(ix * u_x) + (int)(iy * u_y);
			dst->pixels[dst->w * (dst_r.y + y) + (dst_r.x + x)] = KIT_WHITE;
		}
	}
}


int lib_gfx_draw_image(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	kit_Image *src_i;
	kit_Color color_to_add,color_to_mul;
	color_to_add=color_to_mul=KIT_WHITE;
	int flip_x=1,flip_y=1;

	kit_Rect src_r={};
	kit_Rect dst_r={};

	float u_x,u_y;
	u_x = 0;
	u_y = 1;
	int i = 0;
	int nargs = elf_get_num_args(S);
	if (nargs >= 1) { nargs -= 1;
		elf_Table *tab = elf_get_table(S,i++);
		src_i = (kit_Image *) elf_tgets_int(tab,elf_new_string(S,"@ptr"));
		dst_r.w = src_r.w = src_i->w;
		dst_r.h = src_r.h = src_i->h;
	} else goto _nop;
	if (nargs >= 4) { nargs -= 4;
		i = _get_color(S,i,&color_to_mul);
	} else goto _draw;
	if (nargs >= 4) { nargs -= 4;
		i = _get_color(S,i,&color_to_add);
	} else goto _draw;
	if (nargs >= 4) { nargs -= 4;
		i = _get_rect(S,i,&dst_r);
	} else goto _draw;
	if (nargs >= 4) { nargs -= 4;
		i = _get_rect(S,i,&src_r);
	} else goto _draw;
	if (nargs >= 2) { nargs -= 2;
		flip_x = elf_get_int(S,i++) ? -1 : 1;
		flip_y = elf_get_int(S,i++) ? -1 : 1;
	} else goto _draw;
	if (nargs >= 1) { nargs -= 1;
		float rotation = elf_get_num(S,i++);
		u_x = cos(rotation);
		u_y = sin(rotation);
	} else goto _draw;

	_draw:

	#define MIN(A,B) ((A) < (B) ? (A) : (B))
	#define MAX(A,B) ((A) > (B) ? (A) : (B))
	#define CLIP(A,THE_MIN,THE_MAX) MIN(MAX(A,THE_MIN),THE_MAX)

	float r_x,r_y;
	r_x =   u_y;
	r_y = - u_x;
	transform2d src_t = {1,1,0,0};
	if (flip_x == -1) {
		src_t.add_x = src_r.w - 1;
		src_t.mul_x = -1;
	}

	// todo: proper sampling...
	// int xx,yy;
	// if(dst_x < 0) xx = - dst_x, dst_w += dst_x, dst_x = 0; else xx = 0;
	// if(dst_y < 0) yy = - dst_y, dst_h += dst_y, dst_y = 0; else yy = 0;
	// dst_w = CLIP(dst_w, 0, ctx->screen->w - dst_x);
	// dst_h = CLIP(dst_h, 0, ctx->screen->h - dst_y);
	// ASSERT(dst_x + dst_w <= ctx->screen->w);
	// ASSERT(dst_y + dst_h <= ctx->screen->h);
	// ASSERT(dst_x >= 0);
	// ASSERT(dst_y >= 0);

	if (dst_r.h <= 0 || dst_r.w <= 0) goto _nop;

	// ASSERT(src_x + (xx + dst_w) * sx <= src_i->w);
	// ASSERT(src_y + (yy + dst_h) * sy <= src_i->h);

	kit_Image *dst = ctx->screen;

	for (int y = 0; y < dst_r.h; y ++) {
		for (int x = 0; x < dst_r.w; x ++) {
			int d_x = (dst_r.x + x);
			int d_y = (dst_r.y + y);

			if (d_y < 0 || d_y >= dst->h) continue;
			if (d_x < 0 || d_x >= dst->w) continue;

			int sam_x = src_r.x + (src_r.w - 1) * ((float) x / (dst_r.w - 1));
			int sam_y = src_r.y + (src_r.h - 1) * ((float) y / (dst_r.h - 1));

			int tsam_x = ((int)(sam_x * r_x) + (int)(sam_y * r_y)) * src_t.mul_x + src_t.add_x;
			int tsam_y = ((int)(sam_x * u_x) + (int)(sam_y * u_y)) * src_t.mul_y + src_t.add_y;
			if (tsam_y < 0 || tsam_y >= src_i->h) continue;
			if (tsam_x < 0 || tsam_x >= src_i->w) continue;
			kit_Color color = src_i->pixels[src_i->w * tsam_y + tsam_x];
			if (color.a != 0) {
				dst->pixels[dst->w * (dst_r.y + y) + (dst_r.x + x)] = color;
			}
		}
	}
	goto _nop;
	kit_draw_image3(ctx,color_to_mul,color_to_add,src_i,dst_r,src_r);
	_nop:
	return 0;
}


int lib_gfx_draw_line(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	int i = 0;

	int r = elf_get_int(S,i++);
	int g = elf_get_int(S,i++);
	int b = elf_get_int(S,i++);
	int a = elf_get_int(S,i++);

	int x1 = elf_get_int(S,i++);
	int y1 = elf_get_int(S,i++);
	int x2 = elf_get_int(S,i++);
	int y2 = elf_get_int(S,i++);
	kit_draw_line(ctx, (kit_Color){b,g,r,a},x1,y1,x2,y2);
	return 0;
}


int lib_gfx_draw_point(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	int i = 0;

	int r = elf_get_int(S,i++);
	int g = elf_get_int(S,i++);
	int b = elf_get_int(S,i++);
	int a = elf_get_int(S,i++);

	int x1 = elf_get_int(S,i++);
	int y1 = elf_get_int(S,i++);

	kit_draw_point(ctx, (kit_Color){b,g,r,a},x1,y1);
	return 0;
}

int lib_gfx_draw_rect2(elf_State *S) {
	kit_Context *ctx = get_ctx(S);

	int i = 0;
	int x = elf_get_int(S,i++);
	int y = elf_get_int(S,i++);
	int w = elf_get_int(S,i++);
	int h = elf_get_int(S,i++);

	elf_f64 angle = elf_get_num(S,i++);

	int r = elf_get_int(S,i++);
	int g = elf_get_int(S,i++);
	int b = elf_get_int(S,i++);
	int a = elf_get_int(S,i++);


	float u_x = cosf(angle);
	float u_y = sinf(angle);
	float r_x = u_y;
	float r_y = - u_x;

	int dst_w = ctx->screen->w;

	for (int yofs = 0; yofs < h; yofs ++) {
		for (int xofs = 0; xofs < w; xofs ++) {
			int yy = y+(int)(yofs*u_x+yofs*u_y);
			int xx = x+(int)(xofs*r_x+xofs*r_y);
			ctx->screen->pixels[dst_w * yy + xx] = KIT_WHITE;
		}
	}
	return 0;
}