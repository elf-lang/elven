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

#define WHITE   _rgb(255,255,255)
#define RED     _rgb(255,0,0)
#define GREEN   _rgb(0,255,0)
#define BLUE    _rgb(0,0,255)
#define YELLOW  _rgb(255,255,0)
#define CYAN    _rgb(0,255,255)
#define MAGENTA _rgb(255,0,255)

#define _rgb(r,g,b) (kit_Color){b,g,r,255}


typedef struct {float x,y;} f32x2;
typedef struct {float x,y;} i32x2;


static kit_Color _cur_color;
static kit_Color _cur_add_color;
static kit_Image *_cur_src_image;
static kit_Rect _cur_src_rect;
static f32x2 _cur_offset;
static f32x2 _cur_scale;

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

int lib_gfx_draw_rect(elf_State *S);
int lib_gfx_draw_text(elf_State *S);
int lib_gfx_draw_image(elf_State *S);
int lib_gfx_draw_line(elf_State *S);
int lib_gfx_draw_point(elf_State *S);
int lib_gfx_set_color(elf_State *S);
int lib_gfx_set_add_color(elf_State *S);
int lib_gfx_set_src_image(elf_State *S);
int lib_gfx_set_src_rect(elf_State *S);
int lib_gfx_set_offset(elf_State *S);
int lib_gfx_set_scale(elf_State *S);

void lib_gfx_draw_apply(elf_State *S, elf_Table *meta){
	elf_table_set(meta,VSTR(elf_new_string(S,"set_offset")),VCFN(lib_gfx_set_offset));
	elf_table_set(meta,VSTR(elf_new_string(S,"set_scale")),VCFN(lib_gfx_set_scale));
	elf_table_set(meta,VSTR(elf_new_string(S,"set_color")),VCFN(lib_gfx_set_color));
	elf_table_set(meta,VSTR(elf_new_string(S,"set_add_color")),VCFN(lib_gfx_set_add_color));
	elf_table_set(meta,VSTR(elf_new_string(S,"set_src_image")),VCFN(lib_gfx_set_src_image));
	elf_table_set(meta,VSTR(elf_new_string(S,"set_src_rect")),VCFN(lib_gfx_set_src_rect));

	_cur_scale.x = 1;
	_cur_scale.y = 1;

	elf_table_set(meta,VSTR(elf_new_string(S,"draw_rect")),VCFN(lib_gfx_draw_rect));
	elf_table_set(meta,VSTR(elf_new_string(S,"draw_text")),VCFN(lib_gfx_draw_text));
	elf_table_set(meta,VSTR(elf_new_string(S,"draw_image")),VCFN(lib_gfx_draw_image));
	elf_table_set(meta,VSTR(elf_new_string(S,"draw_line")),VCFN(lib_gfx_draw_line));
	elf_table_set(meta,VSTR(elf_new_string(S,"draw_point")),VCFN(lib_gfx_draw_point));
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
	_cur_src_image = (kit_Image *) elf_tgets_int(tab,elf_new_string(S,"@ptr"));
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

int lib_gfx_draw_rect(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	kit_Rect rect;
	_get_rect_trans(S,0,&rect);
	kit_draw_rect(ctx,_cur_color,rect);
	return 0;
}

typedef struct {
	kit_Image *dst;
	kit_Image *src;
	kit_Rect dst_r;
	kit_Rect src_r;
	f32x2 center;
	float angle;
	bool flip_x,flip_y;
} _rect_params;
// not particularly fast, but it seems to get
// the job done...
// todo: we could speed it up tho
static void _rect(_rect_params *params) {
	kit_Image *dst = params->dst;
	kit_Rect dst_r = params->dst_r;
	kit_Image *src = params->src;
	kit_Rect src_r = params->src_r;
	f32x2 center = params->center;
	float angle = params->angle;
	bool flip_x = params->flip_x;
	bool flip_y = params->flip_y;

	f32x2 o = (f32x2){dst_r.x,dst_r.y};

	// generate basis vectors
	f32x2 br = (f32x2){cos(angle),sin(angle)};
	f32x2 bu = (f32x2){-br.y,br.x};

	f32x2 cx = (f32x2){center.x,center.x};
	f32x2 cy = (f32x2){center.y,center.y};

	#define _add(a,b) ((f32x2){(a).x+(b).x,(a).y+(b).y})
	#define _sub(a,b) ((f32x2){(a).x-(b).x,(a).y-(b).y})
	#define _mul(a,b) ((f32x2){(a).x*(b).x,(a).y*(b).y})
	#define _dot(a,b) ((a).x*(b).x + (a).y*(b).y)

	// cast x and y along the basis vectors
	#define _rot(x,y) _add(o,_add(_mul(br,_sub(((f32x2){x,x}),cx)), _mul(bu,_sub(((f32x2){y,y}),cy))))

	// generate the 4 extremes, determine
	// bounding box and clip to the destination
	// rect.
	f32x2 e[4] = {_rot(0,0),_rot(dst_r.w,0),_rot(0,dst_r.h),_rot(dst_r.w,dst_r.h)};

	int x0=0xffff,y0=0xffff,x1=0,y1=0;

	for(int i = 0; i < 4; i ++) {
		f32x2 p = e[i];
		x0=MAX(0,MIN(x0,floor(p.x)));
		y0=MAX(0,MIN(y0,floor(p.y)));
		x1=MIN(dst->w,MAX(x1,ceil(p.x + 1)));
		y1=MIN(dst->h,MAX(y1,ceil(p.y + 1)));
	}

	for (int y = y0; y < y1; y ++) {
		for (int x = x0; x < x1; x ++) {
			// position relative origin
			f32x2 p = (f32x2){x-o.x,y-o.y};

			// project point along basis vectors
			float pr = _dot(br,p) + center.x;
			float pu = _dot(bu,p) + center.y;

			// check if within rect
			if (pr >= 0 && pr < dst_r.w && pu >= 0 && pu < dst_r.h) {
				kit_Color color = (kit_Color){255,0,0,255};
				if (src) {
					// convert to u v space, then scale
					int tex_x = pr / dst_r.w * src->w;
					int tex_y = pu / dst_r.h * src->h;
					if(flip_x) tex_x = src->w - 1 - tex_x;
					if(flip_y) tex_y = src->h - 1 - tex_y;
					color = src->pixels[src->w * tex_y + tex_x];
				}
				if (color.a) {
					dst->pixels[dst->w*y+x]=color;
				}
			}
		}
	}
}


int lib_gfx_draw_image(elf_State *S) {
	kit_Context *ctx = get_ctx(S);
	kit_Image *src_i;
	kit_Color color_to_add,color_to_mul;
	color_to_add=color_to_mul=KIT_WHITE;
	int flip_x=0,flip_y=0;

	kit_Rect src_r={};
	kit_Rect dst_r={};

	float u_x,u_y;
	u_x = 0;
	u_y = 1;
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
			dst_r.y -= dst_r.h;
		}
		// i = _get_rect_trans(S,i,&dst_r);
	} else goto _nop;

	src_r = _cur_src_rect;
#if 0
	if (nargs >= 4) { nargs -= 4;
		i = _get_rect(S,i,&src_r);
	} else goto _draw;
#endif

	if (nargs >= 2) { nargs -= 2;
		flip_x = elf_get_int(S,i++);
		flip_y = elf_get_int(S,i++);
	} else goto _draw;
	if (nargs >= 1) { nargs -= 1;
		float rotation = elf_get_num(S,i++);
		u_x = cos(rotation);
		u_y = sin(rotation);
	} else goto _draw;

	_draw:

	_rect_params params = {};
	params.dst = ctx->screen;
	params.src = src_i;
	params.dst_r = dst_r;
	params.src_r = src_r;
	params.center = (f32x2){0,0};
	params.angle = 0;
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

#if 0
	int w = src->w * 4; // 32;
	int h = src->h * 4; // 64;
	f32x2 c = (f32x2){w*0.5,h*0.9};
	// f32x2 c = (f32x2){};
	// f32x2 c = (f32x2){w*0.5,0};

	// generate basis vectors
	f32x2 br = (f32x2){cos(time),sin(time)};
	f32x2 bu = (f32x2){-br.y,br.x};

	f32x2 cx = (f32x2){c.x,c.x};
	f32x2 cy = (f32x2){c.y,c.y};

	#define _add(a,b) ((f32x2){(a).x+(b).x,(a).y+(b).y})
	#define _sub(a,b) ((f32x2){(a).x-(b).x,(a).y-(b).y})
	#define _mul(a,b) ((f32x2){(a).x*(b).x,(a).y*(b).y})
	#define _dot(a,b) ((a).x*(b).x + (a).y*(b).y)

	// cast x and y along the basis vectors
	#define _rot(x,y) _add(o,_add(_mul(br,_sub(((f32x2){x,x}),cx)) \
	,									   _mul(bu,_sub(((f32x2){y,y}),cy))))

	// generate the 4 extremes, determine
	// bounding box and clip to the destination rect.
	f32x2 e[4] = {_rot(0,0),_rot(w,0),_rot(0,h),_rot(w,h)};
	int x0=0xffff,y0=0xffff,x1=0,y1=0;
	for(int i = 0; i < 4; i ++) {
		f32x2 p = e[i];

		x0=MAX(0,MIN(x0,floor(p.x)));
		y0=MAX(0,MIN(y0,floor(p.y)));
		x1=MIN(dst->w,MAX(x1,ceil(p.x + 1)));
		y1=MIN(dst->h,MAX(y1,ceil(p.y + 1)));

		// visualize
		kit_Color colors[] = {RED,GREEN,BLUE,CYAN};
		kit_draw_rect(test, colors[i], (kit_Rect){p.x-2,p.y-2,4,4});
	}

	// disable culling
	x0 = 0,y0 = 0,x1 = 320,y1 = 240;

	// iterate over every pixel on the bounding box,
	// determine if inside of rectangle
	for (int y = y0; y < y1; y ++) {
		for (int x = x0; x < x1; x ++) {
			// position relative origin
			f32x2 p = (f32x2){x-o.x,y-o.y};

			// project point along basis vectors
			float pr = _dot(br,p) + c.x;
			float pu = _dot(bu,p) + c.y;
			// check if within rect
			if (pr >= 0 && pr < w && pu >= 0 && pu < h) {
				kit_Color color = (kit_Color){255,0,0,255};
				if (src) {
					// convert to u v space, then scale
					int tex_x = pr / w * src->w;
					int tex_y = pu / h * src->h;
					// tex_y = (src->h - 1) - tex_y;
					color = src->pixels[src->w * tex_y + tex_x];
				}
				if (color.a) {
					kit_draw_point(test, color, x,y);
				}
			}
		}
	}

	{
		f32x2 eu = (f32x2){bu.x*h,bu.y*h};
		f32x2 er = (f32x2){br.x*w,br.y*w};

		kit_draw_line(test, _rgb(0,255,0), o.x,o.y,o.x+eu.x,o.y+eu.y);
		kit_draw_line(test, _rgb(255,0,0), o.x,o.y,o.x+er.x,o.y+er.y);

		f32x2 p = (f32x2){test->mouse_pos.x-o.x,test->mouse_pos.y-o.y};

		float pr = (er.x*p.x + er.y*p.y) / (w*w);
		f32x2 dr = (f32x2){er.x * pr,er.y * pr};

		float pu = (eu.x*p.x + eu.y*p.y) / (h*h);
		f32x2 du = (f32x2){eu.x * pu,eu.y * pu};

		kit_draw_rect(test, (kit_Color){255,255,0,64}, (kit_Rect){x0,y0,x1-x0,y1-y0});


		_text("ru: %.2f,%.2f",pr,pu);
		_text("bu: %.2f,%.2f",bu.x,bu.y);
		_text("br: %.2f,%.2f",br.x,br.y);

		kit_draw_point(test, (kit_Color){0,255,0,255}, test->mouse_pos.x,test->mouse_pos.y);
		kit_draw_rect(test, (kit_Color){255,255,0,255}, (kit_Rect){o.x+du.x-2,o.y+du.y-2,4,4});
		kit_draw_rect(test, (kit_Color){255,255,0,255}, (kit_Rect){o.x+dr.x-2,o.y+dr.y-2,4,4});
	}
#endif