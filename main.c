#define ELF_KEEPWINDOWS
#define ELF_NOMAIN
#include "elf\elf-web.c"
#include "timeapi.h"
#define KIT_IMPL
#include "kit\kit.h"

typedef elf_State  elState;
typedef elf_String elString;
typedef elf_Int    elInteger;

double dt;
kit_Context *ctx;

int elf_kit_create_image(elState *S) {
	int w = elf_get_int(S,0);
	int h = elf_get_int(S,1);
	kit_Image *img = kit_create_image(w,h);

	elf_Table *tab = elf_new_table(S);
	elf_tsets_int(tab,elf_new_string(S,"handle"),(elf_Int)(img));
	elf_tsets_int(tab,elf_new_string(S,"width"),w);
	elf_tsets_int(tab,elf_new_string(S,"height"),h);
	elf_add_tab(S,tab);
	return 1;
}
int elf_kit_load_image_file(elState *S) {
	char *filename = elf_get_txt(S,0);
	kit_Image *img = kit_load_image_file(filename);

	elf_Table *tab = elf_new_table(S);
	elf_tsets_int(tab,elf_new_string(S,"handle"),(elf_Int)(img));
	elf_tsets_int(tab,elf_new_string(S,"width"),img->w);
	elf_tsets_int(tab,elf_new_string(S,"height"),img->h);
	elf_add_tab(S,tab);
	return 1;
}

int elf_kit_draw_image(elState *S) {
	kit_Image *img;
	int x=0,y=0;
	int r=255,g=255,b=255,a=255;
	int src_x=0,src_y=0,src_w,src_h;
	int i = 0;
	int nargs = elf_get_num_args(S);
	if (nargs >= 1) { nargs -= 1;
		elf_Table *tab = elf_get_tab(S,i++);
		img = (kit_Image *) elf_tgets_int(tab,elf_new_string(S,"handle"));
		src_w = img->w;
		src_h = img->h;
	} else goto _nop;
	if (nargs >= 2) { nargs -= 2;
		x = elf_get_int(S,i++);
		y = elf_get_int(S,i++);
	} else goto _nop;
	if (nargs >= 3) { nargs -= 3;
		r = elf_get_int(S,i++);
		g = elf_get_int(S,i++);
		b = elf_get_int(S,i++);
	} else goto _draw;
	if (nargs >= 1) { nargs -= 1;
		a = elf_get_int(S,i++);
	} else goto _draw;
	if (nargs >= 4) { nargs -= 4;
		src_x = elf_get_int(S,i++);
		src_y = elf_get_int(S,i++);
		src_w = elf_get_int(S,i++);
		src_h = elf_get_int(S,i++);
	} else goto _draw;

	_draw:
	kit_draw_image2(ctx,(kit_Color){r,g,b,a},img,x,y,(kit_Rect){src_x,src_y,src_w,src_h});
	_nop:
	return 0;
}

kit_Image * kit_load_image_mem(void *data, int len);
void       kit_destroy_image(kit_Image *img);


// int elf_kit_get_char(elState *S);
int elf_kit_key_down(elState *S) {
	int key = elf_get_int(S,0);
	elf_add_int(S,kit_key_down(ctx,key));
	return 1;
}
int elf_kit_key_pressed(elState *S) {
	int key = elf_get_int(S,0);
	elf_add_int(S,kit_key_pressed(ctx,key));
	return 1;
}
int elf_kit_key_released(elState *S) {
	int key = elf_get_int(S,0);
	elf_add_int(S,kit_key_released(ctx,key));
	return 1;
}
// int elf_kit_mouse_pos(elState *S, int *x, int *y);
// int elf_kit_mouse_delta(elState *S, int *x, int *y);
// int elf_kit_mouse_down(elState *S, int button);
// int elf_kit_mouse_pressed(elState *S, int button);
// int elf_kit_mouse_released(elState *S, int button);


int elf_kit_create(elState *S) {
	elString *name = elf_get_str(S,0);
	int w = elf_get_int(S,1);
	int h = elf_get_int(S,2);
	ctx = kit_create(name->text, 256, 128, KIT_SCALE4X|KIT_FPS144);
	return 0;
}

int elf_kit_step(elState *S) {

	// for(int i =0; i < 100; i ++) {
	// 	kit_draw_rect(ctx,(kit_Color){255,255,255,255},(kit_Rect){0,0,200,200});
	// }
	kit_draw_text(ctx,KIT_BLACK,elf_tpf("FPS: %.2f",1.0/dt),100,0);
	kit_draw_text(ctx,KIT_WHITE,elf_tpf("FPS: %.2f",1.0/dt),100,1);

	int b = kit_step(ctx, &dt);
	kit_clear(ctx,KIT_BLACK);
	elf_add_int(S,b);
	return 1;
}

int elf_kit_get_delta_time(elState *S) {
	elf_add_num(S,dt);
	return 1;
}

int elf_kit_draw_rect(elState *S) {
	int x = elf_get_int(S,0);
	int y = elf_get_int(S,1);
	int w = elf_get_int(S,2);
	int h = elf_get_int(S,3);
	int r = elf_get_int(S,4);
	int g = elf_get_int(S,5);
	int b = elf_get_int(S,6);
	int a = elf_get_int(S,7);
	kit_draw_rect(ctx,(kit_Color){b,g,r,a},(kit_Rect){x,y,w,h});
	return 0;
}

int elf_kit_draw_text(elState *S) {
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


int main() {
	elf_global_initialize();
	elf_gsetx_cfn(&elf.R,"elf.kit.is_key_down",elf_kit_key_down);
	elf_gsetx_cfn(&elf.R,"elf.kit.is_key_pressed",elf_kit_key_pressed);
	elf_gsetx_cfn(&elf.R,"elf.kit.is_key_released",elf_kit_key_released);
	elf_gsetx_cfn(&elf.R,"elf.kit.create",elf_kit_create);
	elf_gsetx_cfn(&elf.R,"elf.kit.step",elf_kit_step);
	elf_gsetx_cfn(&elf.R,"elf.kit.draw_rect",elf_kit_draw_rect);
	elf_gsetx_cfn(&elf.R,"elf.kit.draw_text",elf_kit_draw_text);
	elf_gsetx_cfn(&elf.R,"elf.kit.get_delta_time",elf_kit_get_delta_time);
	elf_gsetx_cfn(&elf.R,"elf.kit.create_image",elf_kit_create_image);
	elf_gsetx_cfn(&elf.R,"elf.kit.load_image_file",elf_kit_load_image_file);
	elf_gsetx_cfn(&elf.R,"elf.kit.draw_image",elf_kit_draw_image);

	elf_add_cfn(&elf.R,core_lib_load_file);
	elf_add_nil(&elf.R);
	elf_new_string(&elf.R,"launch.elf");
	elf_call_function(&elf.R,2,0);
}