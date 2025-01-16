// todo: should be more of a frame buffer thing

int lib_gfx_draw_rect(elf_State *S);
int lib_gfx_draw_text(elf_State *S);
int lib_gfx_draw_image(elf_State *S);
int lib_gfx_draw_rect2(elf_State *S);
int lib_gfx_draw_line(elf_State *S);
int lib_gfx_draw_point(elf_State *S);

void lib_gfx_draw_apply(elf_State *S, elf_Table *meta){
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

kit_Context *get_ctx(elf_State *S){
	elf_Table *_this;
	_this = (elf_Table *) elf_get_this(S);

	kit_Context *ctx;
	ctx=(kit_Context *)elf_table_get(_this
	, VSTR(elf_new_string(S,"@ptr"))).x_int;
	ASSERT(ctx != 0);
	return ctx;
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


int lib_gfx_draw_image(elf_State *S) {
	kit_Context *ctx = get_ctx(S);

	kit_Image *img;
	int mul_r=255,mul_g=255,mul_b=255,mul_a=255;
	int add_r=255,add_g=255,add_b=255,add_a=255;
	int src_x=0,src_y=0,src_w=0,src_h=0;
	int dst_x=0,dst_y=0,dst_w=0,dst_h=0;
	int flip_x=1,flip_y=1;
	int i = 0;

	int nargs = elf_get_num_args(S);
	if (nargs >= 1) { nargs -= 1;
		elf_Table *tab = elf_get_table(S,i++);
		img = (kit_Image *) elf_tgets_int(tab,elf_new_string(S,"@ptr"));
		dst_w = src_w = img->w;
		dst_h = src_h = img->h;
	} else goto _nop;
	if (nargs >= 4) { nargs -= 4;
		mul_r = elf_get_int(S,i++);
		mul_g = elf_get_int(S,i++);
		mul_b = elf_get_int(S,i++);
		mul_a = elf_get_int(S,i++);
	} else goto _draw;
	if (nargs >= 4) { nargs -= 4;
		add_r = elf_get_int(S,i++);
		add_g = elf_get_int(S,i++);
		add_b = elf_get_int(S,i++);
		add_a = elf_get_int(S,i++);
	} else goto _draw;
	if (nargs >= 4) { nargs -= 4;
		dst_x = elf_get_int(S,i++);
		dst_y = elf_get_int(S,i++);
		dst_w = elf_get_int(S,i++);
		dst_h = elf_get_int(S,i++);
	} else goto _draw;
	if (nargs >= 4) { nargs -= 4;
		src_x = elf_get_int(S,i++);
		src_y = elf_get_int(S,i++);
		src_w = elf_get_int(S,i++);
		src_h = elf_get_int(S,i++);
	} else goto _draw;
	if (nargs >= 2) { nargs -= 2;
		flip_x = elf_get_int(S,i++) ? -1 : 1;
		flip_y = elf_get_int(S,i++) ? -1 : 1;
	} else goto _draw;

	_draw:
	#define MIN(A,B) ((A) < (B) ? (A) : (B))
	#define MAX(A,B) ((A) > (B) ? (A) : (B))
	#define CLIP(A,THE_MIN,THE_MAX) MIN(MAX(A,THE_MIN),THE_MAX)

	// Todo: has to be made better!
	float sx,sy;
	sx = src_w / (float) dst_w;
	sy = src_h / (float) dst_h;

	int xx,yy;
	if(dst_x < 0) xx = - dst_x, dst_w += dst_x, dst_x = 0; else xx = 0;
	if(dst_y < 0) yy = - dst_y, dst_h += dst_y, dst_y = 0; else yy = 0;

	dst_w = CLIP(dst_w, 0, ctx->screen->w - dst_x);
	dst_h = CLIP(dst_h, 0, ctx->screen->h - dst_y);
	ASSERT(dst_x + dst_w <= ctx->screen->w);
	ASSERT(dst_y + dst_h <= ctx->screen->h);
	ASSERT(dst_x >= 0);
	ASSERT(dst_y >= 0);

	if (dst_h <= 0 || dst_w <= 0) goto _nop;

	ASSERT(src_x + (xx + dst_w) * sx <= img->w);
	ASSERT(src_y + (yy + dst_h) * sy <= img->h);


	float tex_y; // = src_y + yy * sy;
	if(flip_y == -1) tex_y = src_y + src_h - 1 - yy * sy;
	else 				  tex_y = src_y + 		      yy * sy;

	for (int y = 0; y < dst_h; y ++) {
		float tex_x;
		if(flip_x == -1) tex_x = src_x + src_w - 1 - xx * sx;
		else 				  tex_x = src_x + 		      xx * sx;
		for (int x = 0; x < dst_w; x ++) {
			kit_Color color = img->pixels[img->w * ((int) tex_y) + ((int) tex_x)];
			if (color.a != 0) {
				ctx->screen->pixels[ctx->screen->w * (dst_y + y) + (dst_x + x)] = color;
			}
			tex_x += sx * flip_x;
		}
		tex_y += sy * flip_y;
	}
	goto _nop;
	kit_draw_image3(ctx
	,(kit_Color){mul_b,mul_g,mul_r,mul_a}
	,(kit_Color){add_b,add_g,add_r,add_a}, img
	,(kit_Rect){dst_x,dst_y,dst_w,dst_h}
	,(kit_Rect){src_x,src_y,src_w,src_h});
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