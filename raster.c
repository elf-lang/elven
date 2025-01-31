
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

#define _add(a,b) ((f32x2){(a).x+(b).x,(a).y+(b).y})
#define _sub(a,b) ((f32x2){(a).x-(b).x,(a).y-(b).y})
#define _mul(a,b) ((f32x2){(a).x*(b).x,(a).y*(b).y})
#define _dot(a,b) ((a).x*(b).x + (a).y*(b).y)

void _set(kit_Image *img, int x, int y, kit_Color color) {
	if (x >= 0 && x < img->w && y >= 0 && y < img->h) {
		img->pixels[img->w * y + x] = color;
	}
}

typedef struct {
	kit_Image *dst;
	kit_Image *src;
	kit_Rect   dst_r;
	kit_Rect   src_r;
	f32x2      center;
	float      angle;
	bool       flip_x,flip_y;
} _rect_params;

// not particularly fast...
static void _quad(_rect_params *params) {
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

	// todo: if we could figure out what the starting
	// position is per scanline and the length of the
	// scanline...
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
					if(flip_x) pr = dst_r.w - 1 - pr;
					if(flip_y) pu = dst_r.h - 1 - pu;
					// convert to u v space, then scale
					int tex_x = pr / dst_r.w * src_r.w;
					int tex_y = pu / dst_r.h * src_r.h;
					tex_x += src_r.x;
					tex_y += src_r.y;
					color = src->pixels[src->w * tex_y + tex_x];
				}
				if (color.a) {
					dst->pixels[dst->w*y+x]=color;
				}
			}
		}
	}
}

void _circle(kit_Image *dst, int x0, int y0, int r, kit_Color color) {
	int E = 1 - r;
	int dx = 0;
	int dy = -2 * r;
	int x = 0;
	int y = r;

	_set(dst, x0, y0 + r, color);
	_set(dst, x0, y0 - r, color);
	_set(dst, x0 + r, y0, color);
	_set(dst, x0 - r, y0, color);

	while (x < y - 1) {
		x++;

		if (E >= 0) {
			y--;
			dy += 2;
			E += dy;
		}

		dx += 2;
		E += dx + 1;

		_set(dst, x0 + x, y0 + y, color);
		_set(dst, x0 - x, y0 + y, color);
		_set(dst, x0 + x, y0 - y, color);
		_set(dst, x0 - x, y0 - y, color);

		if (x != y) {
			_set(dst, x0 + y, y0 + x, color);
			_set(dst, x0 - y, y0 + x, color);
			_set(dst, x0 + y, y0 - x, color);
			_set(dst, x0 - y, y0 - x, color);
		}
	}
}


void raster_demo() {
	kit_Context *test = kit_create("test",320,240,KIT_SCALE2X);
	double prev = 0;
	double time = 0;
	kit_Image *src = kit_load_image_file("W:\\MicroRJ\\platformer\\data\\images\\entities\\player.png");
	kit_Image *dst = test->screen;
	f32x2 o = (f32x2){160,120};
	while (kit_step(test,&prev)) {
		kit_clear(test,KIT_BLACK);
		if(kit_key_down(test,'A'))time -= 0.01;
		if(kit_key_down(test,'D'))time += 0.01;

		int w = src->w * 4;
		int h = src->h * 4;
		f32x2 c = (f32x2){};

		f32x2 br = (f32x2){cos(time),sin(time)};
		f32x2 bu = (f32x2){-br.y,br.x};

		f32x2 cx = (f32x2){c.x,c.x};
		f32x2 cy = (f32x2){c.y,c.y};

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

			int _text_y = 0;
#define _text(text,...) kit_draw_text(test, KIT_WHITE, elf_tpf(text,__VA_ARGS__), 0,_text_y), _text_y += 15;

			_text("ru: %.2f,%.2f",pr,pu);
			_text("bu: %.2f,%.2f",bu.x,bu.y);
			_text("br: %.2f,%.2f",br.x,br.y);
			_text("fps: %.2f",1.0/prev);

			kit_draw_point(test, (kit_Color){0,255,0,255}, test->mouse_pos.x,test->mouse_pos.y);
			kit_draw_rect(test, (kit_Color){255,255,0,255}, (kit_Rect){o.x+du.x-2,o.y+du.y-2,4,4});
			kit_draw_rect(test, (kit_Color){255,255,0,255}, (kit_Rect){o.x+dr.x-2,o.y+dr.y-2,4,4});
		}
	}
}