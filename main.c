#define _DEBUG
#include "elf\elf.c"
#include "timeapi.h"
#define KIT_IMPL
#include "kit\kit.h"
#include "lib_image.c"
#include "lib_draw.c"
#include "lib_window.c"
#include "lib_audio.c"

#include "package.c"



int main() {
	kit_Context *test = kit_create("test",320,240,KIT_SCALE4X);
	double prev = 0;
	double time = 0;
	kit_Image *src = kit_load_image_file("W:\\MicroRJ\\platformer\\data\\images\\entities\\player.png");
	kit_Image *dst = test->screen;
	f32x2 o = (f32x2){160,120};

	while (kit_step(test,&prev)) {
		kit_clear(test,KIT_BLACK);

		if(kit_key_down(test,'A')){
			time -= 0.01;
			// o.x -= 1;
		}
		if(kit_key_down(test,'D')){
			time += 0.01;
			// o.x += 1;
		}
		int w = src->w; // 32;
		int h = src->h; // 64;
		f32x2 c = (f32x2){w*0.5,h*0.5};
		// f32x2 c = (f32x2){};

		// generate basis vectors
		f32x2 br = (f32x2){cos(time),sin(time)};
		f32x2 bu = (f32x2){br.y,-br.x};

		// generate the 4 extremes points, determine
		// bounding box and clip to the destination rect.
		i32x2 rp[4] = {{0,0},{w,0},{0,h},{w,h}};
		int x0=0xffff,y0=0xffff,x1=0,y1=0;
		kit_Color colors[] = {(kit_Color){255,0,0,255},(kit_Color){0,255,0,255},(kit_Color){0,0,255,255}
		,	(kit_Color){255,255,0,255}};
		for(int i = 0; i < 4; i ++) {
			i32x2 p = (i32x2){rp[i].x - c.x,rp[i].y - c.y};
			f32x2 t = (f32x2){
				br.x*p.x + br.y*p.y + o.x,
				bu.x*p.x + bu.y*p.y + o.y,
			};

			x0=MAX(0,MIN(x0,floor(t.x)));
			y0=MAX(0,MIN(y0,floor(t.y)));
			x1=MIN(dst->w,MAX(x1,ceil(t.x + 1)));
			y1=MIN(dst->h,MAX(y1,ceil(t.y + 1)));

			// visualize
			kit_draw_rect(test, colors[i], (kit_Rect){t.x-2,t.y-2,4,4});
		}

		// disable culling
		x0 = 0,y0 = 0,x1 = 320,y1 = 240;

		// iterate over every pixel on the bounding box,
		// determine if inside of rectangle
		for (int y = y0; y < y1; y ++) {
			for (int x = x0; x < x1; x ++) {
				// position relative origin
				f32x2 p = (f32x2){x-o.x,y-o.y};
				float pr = (br.x*p.x + br.y*p.y + c.x) / (w - 1);
				float pu = (bu.x*p.x + bu.y*p.y + c.y) / (h - 1);
				if (pr >= 0 && pr <= 1.0 && pu >= 0 && pu <= 1.0) {
					kit_Color color = (kit_Color){255,0,0,255};
					if (src) {
						int tex_x = pr * (src->w - 1);
						int tex_y = (src->h - 1) - pu * (src->h - 1);
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

			f32x2 p = (f32x2){test->mouse_pos.x-o.x,test->mouse_pos.y-o.y};

			float pr = (er.x*p.x + er.y*p.y) / (w*w);
			f32x2 dr = (f32x2){er.x * pr,er.y * pr};

			float pu = (eu.x*p.x + eu.y*p.y) / (h*h);
			f32x2 du = (f32x2){eu.x * pu,eu.y * pu};

			kit_draw_rect(test, (kit_Color){255,255,0,64}, (kit_Rect){x0,y0,x1-x0,y1-y0});

			kit_draw_text(test, KIT_WHITE, elf_tpf("u,v: %.2f,%.2f",pr,pu), 0,0);
			kit_draw_line(test, KIT_WHITE, o.x,o.y,o.x+eu.x,o.y+eu.y);
			kit_draw_line(test, KIT_WHITE, o.x,o.y,o.x+er.x,o.y+er.y);
			kit_draw_point(test, (kit_Color){0,255,0,255}, test->mouse_pos.x,test->mouse_pos.y);
			kit_draw_rect(test, (kit_Color){255,255,0,255}, (kit_Rect){o.x+du.x-2,o.y+du.y-2,4,4});
			kit_draw_rect(test, (kit_Color){255,255,0,255}, (kit_Rect){o.x+dr.x-2,o.y+dr.y-2,4,4});
		}


		// draw_params p = {};
		// p.basis_y = (f32x2){cos(time*3.14*0.1),sin(time*3.14*0.1)};
		// // p.basis_y = (f32x2){0,1};
		// p.dst = test->screen;
		// p.dst_r = (kit_Rect){160-32,120-32,64,64};
		// _rect(&p);
	}

	elf_State R = {};
	elf_Module M = {};
	elf_init(&R,&M);
	lib_window_include(&R);
	lib_image_include(&R);
	elf_add_lib(&R,"elf.package",lib_package,_countof(lib_package));
	elf_add_lib(&R,"elf.audio",lib_audio,_countof(lib_audio));
	elf_push_proc(&R,core_lib_load_file);
	elf_push_nil(&R);
	elf_new_string(&R,"launch.elf");
	elf_call(&R,2,0);
}