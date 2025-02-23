#define WHITE   _rgb(255,255,255)
#define RED     _rgb(255,0,0)
#define GREEN   _rgb(0,255,0)
#define BLUE    _rgb(0,0,255)
#define YELLOW  _rgb(255,255,0)
#define CYAN    _rgb(0,255,255)
#define MAGENTA _rgb(255,0,255)

#define _rgb(r,g,b) (kit_Color){b,g,r,255}

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
	vec2      center;
	kit_Color  color;
	kit_Color  color_add;
	float      angle;
	unsigned flip_x: 1;
	unsigned flip_y: 1;
} Raster_Params;

// not particularly fast...
static void _raster(Raster_Params *params) {
	kit_Image *dst = params->dst;
	kit_Rect dst_r = params->dst_r;
	kit_Image *src = params->src;
	kit_Rect src_r = params->src_r;
	vec2 center = params->center;
	float angle = params->angle;
	bool flip_x = params->flip_x;
	bool flip_y = params->flip_y;

	vec2 o = (vec2){dst_r.x,dst_r.y};

	// generate basis vectors
	vec2 br = (vec2){cos(angle),sin(angle)};
	vec2 bu = (vec2){-br.y,br.x};

	vec2 cx = (vec2){center.x,center.x};
	vec2 cy = (vec2){center.y,center.y};

	// cast x and y along the basis vectors
	#define _rot(x,y) vec2_add(o,vec2_add(vec2_mul(br,vec2_sub(((vec2){x,x}),cx)), vec2_mul(bu,vec2_sub(((vec2){y,y}),cy))))

	// generate the 4 extremes, determine
	// bounding box and clip to the destination
	// rect.
	vec2 e[4] = {_rot(0,0),_rot(dst_r.w,0),_rot(0,dst_r.h),_rot(dst_r.w,dst_r.h)};

	int x0=0xffff,y0=0xffff,x1=0,y1=0;

	for(int i = 0; i < 4; i ++) {
		vec2 p = e[i];
		x0=MAX(0,MIN(x0,floor(p.x)));
		y0=MAX(0,MIN(y0,floor(p.y)));
		x1=MIN(dst->w,MAX(x1,ceil(p.x + 1)));
		y1=MIN(dst->h,MAX(y1,ceil(p.y + 1)));
	}

	// pre-compute some terms
	float i_u = 1.f / dst_r.w * src_r.w;
	float i_v = 1.f / dst_r.h * src_r.h;

	// todo: investigate how this could be
	// made faster...
	for (int y = y0; y < y1; y ++) {
		for (int x = x0; x < x1; x ++) {
			// position relative origin
			vec2 p = (vec2){x-o.x,y-o.y};

			// project point along basis vectors
			float pr = v2_dot(br,p) + center.x;
			float pu = v2_dot(bu,p) + center.y;

			// check if within rect
			if (pr >= 0 && pr < dst_r.w && pu >= 0 && pu < dst_r.h) {
				kit_Color color = params->color;
				if (src) {
					if(flip_x) pr = dst_r.w - 1 - pr;
					if(flip_y) pu = dst_r.h - 1 - pu;
					int tex_x = src_r.x + pr * i_u;
					int tex_y = src_r.y + pu * i_v;
					kit_Color sample = src->pixels[src->w * tex_y + tex_x];
					// there has to be some other way of doing this
					const float k = 1.f/255 * 1.f/255 * 255.f;
					color.r = (color.r * sample.r) * k;
					color.g = (color.g * sample.g) * k;
					color.b = (color.b * sample.b) * k;
					color.a = (color.a * sample.a) * k;
				}
				if(color.a){
					kit_Color src_color = dst->pixels[dst->w * y + x];
					// do blending
					const float k = color.a / 255.f;
					color.r = src_color.r + (color.r - src_color.r) * k;
					color.g = src_color.g + (color.g - src_color.g) * k;
					color.b = src_color.b + (color.b - src_color.b) * k;
					color.a = 255;
					dst->pixels[dst->w * y + x] = color;
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
	Graphics_Window *test = kit_create("test",320,240,KIT_SCALE2X);
	double prev = 0;
	double time = 0;
	kit_Image *src = kit_load_image_file("W:\\MicroRJ\\platformer\\data\\images\\entities\\player.png");
	kit_Image *dst = test->screen;
	vec2 o = (vec2){160,120};
	while (kit_step(test,&prev)) {
		kit_clear(test,KIT_BLACK);
		if(kit_key_down(test,'A'))time -= 0.01;
		if(kit_key_down(test,'D'))time += 0.01;

		int w = src->w * 4;
		int h = src->h * 4;
		vec2 c = (vec2){};

		vec2 br = (vec2){cos(time),sin(time)};
		vec2 bu = (vec2){-br.y,br.x};

		vec2 cx = (vec2){c.x,c.x};
		vec2 cy = (vec2){c.y,c.y};

		// generate the 4 extremes, determine
		// bounding box and clip to the destination rect.
		vec2 e[4] = {_rot(0,0),_rot(w,0),_rot(0,h),_rot(w,h)};
		int x0=0xffff,y0=0xffff,x1=0,y1=0;
		for(int i = 0; i < 4; i ++) {
			vec2 p = e[i];

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
				vec2 p = (vec2){x-o.x,y-o.y};

				// project point along basis vectors
				float pr = v2_dot(br,p) + c.x;
				float pu = v2_dot(bu,p) + c.y;
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
			vec2 eu = (vec2){bu.x*h,bu.y*h};
			vec2 er = (vec2){br.x*w,br.y*w};

			kit_draw_line(test, _rgb(0,255,0), o.x,o.y,o.x+eu.x,o.y+eu.y);
			kit_draw_line(test, _rgb(255,0,0), o.x,o.y,o.x+er.x,o.y+er.y);

			vec2 p = (vec2){test->mouse_pos.x-o.x,test->mouse_pos.y-o.y};

			float pr = (er.x*p.x + er.y*p.y) / (w*w);
			vec2 dr = (vec2){er.x * pr,er.y * pr};

			float pu = (eu.x*p.x + eu.y*p.y) / (h*h);
			vec2 du = (vec2){eu.x * pu,eu.y * pu};

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