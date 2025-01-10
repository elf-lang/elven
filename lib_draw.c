// todo: should be more of a frame buffer thing

int lib_gfx_draw_rect(elState *S);
int lib_gfx_draw_text(elState *S);
int lib_gfx_draw_image(elState *S);
int lib_gfx_draw_rect2(elState *S);
int lib_gfx_draw_line(elState *S);

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


int lib_gfx_draw_rect(elState *S) {
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


int lib_gfx_draw_text(elState *S) {
	kit_Context *ctx = get_ctx(S);

	int i = 0;
	elString *string = elf_get_str(S,i++);
	int x = elf_get_int(S,i++);
	int y = elf_get_int(S,i++);
	int r = elf_get_int(S,i++);
	int g = elf_get_int(S,i++);
	int b = elf_get_int(S,i++);
	int a = elf_get_int(S,i++);
	kit_draw_text(ctx,(kit_Color){b,g,r,a},string->text,x,y);
	return 0;
}


int lib_gfx_draw_image(elState *S) {
	kit_Context *ctx = get_ctx(S);

	kit_Image *img;
	int mul_r=255,mul_g=255,mul_b=255,mul_a=255;
	int add_r=255,add_g=255,add_b=255,add_a=255;
	int src_x=0,src_y=0,src_w=0,src_h=0;
	int dst_x=0,dst_y=0,dst_w=0,dst_h=0;
	int i = 0;

	int nargs = elf_get_num_args(S);
	if (nargs >= 1) { nargs -= 1;
		elf_Table *tab = elf_get_tab(S,i++);
		img = (kit_Image *) elf_tgets_int(tab,elf_new_string(S,"handle"));
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

	_draw:
	kit_draw_image3(ctx
	,(kit_Color){mul_b,mul_g,mul_r,mul_a}
	,(kit_Color){add_b,add_g,add_r,add_a}, img
	,(kit_Rect){dst_x,dst_y,dst_w,dst_h}
	,(kit_Rect){src_x,src_y,src_w,src_h});
	_nop:
	return 0;
}


int lib_gfx_draw_line(elState *S) {
	kit_Context *ctx = get_ctx(S);

	int i = 0;
	int x = elf_get_int(S,i++);
	int y = elf_get_int(S,i++);
	elf_f64 angle = elf_get_num(S,i++);
	int l = elf_get_int(S,i++);

	float u_x = cosf(angle);
	float u_y = sinf(angle);

	int dst_w = ctx->screen->w;

	for (int i = 0; i < l; i ++) {
		int yy = y+(int)(i*u_x*i);
		int xx = x+(int)(i*u_y*i);
		ctx->screen->pixels[dst_w * yy + xx] = KIT_WHITE;
	}
	return 0;
}

int lib_gfx_draw_rect2(elState *S) {
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