#define _DEBUG
#define ELF_KEEPWINDOWS
#define ELF_NOMAIN
#include "elf\elf-web.c"
#include "timeapi.h"
#define KIT_IMPL
#include "kit\kit.h"
#include "windows.h"

typedef elf_State  elState;
typedef elf_String elString;
typedef elf_Int    elInteger;

double dt;

void tseti(elState *S, elf_Table *table, char *name, elf_i64 value) {
	elf_table_set(table,VSTR(elf_new_string(S,name)),VINT(value));
}

void upd(elf_Table *w, kit_Context *c){
	tseti(S,w,"@ptr",(elf_i64)c);
	tseti(S,w,"w",c->win_w);
	tseti(S,w,"h",c->win_h);
}

kit_Context *ctx;
int elf_kit_create(elState *S) {
	char *name = elf_get_txt(S,0);
	int w = elf_get_int(S,1);
	int h = elf_get_int(S,2);
	kit_Context *c=ctx=kit_create(name, w, h, KIT_SCALE2X|KIT_FPS144);
	elf_Table *window=elf_new_table(S);
	upd(window,c);
	return 1;
}

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
int elf_kit_load_image(elState *S) {
	char *name = elf_get_txt(S,0);
	kit_Image *img = kit_load_image_file(name);
	elf_Table *tab = elf_new_table(S);
	elf_tsets_int(tab,elf_new_string(S,"handle"),(elf_Int)(img));
	elf_tsets_int(tab,elf_new_string(S,"width"),!img ? 0 : img->w);
	elf_tsets_int(tab,elf_new_string(S,"height"),!img ? 0 : img->h);
	elf_add_tab(S,tab);
	return 1;
}

/* Todo: for speed, might be worth splitting this up with state */
int elf_kit_draw_image(elState *S) {
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

kit_Image * kit_load_image_mem(void *data, int len);
void       kit_destroy_image(kit_Image *img);



int elf_kit_step(elState *S) {



	// for(int i =0; i < 100; i ++) {
	// 	kit_draw_rect(ctx,(kit_Color){255,255,255,255},(kit_Rect){0,0,200,200});
	// }
	kit_draw_text(ctx,KIT_BLACK,elf_tpf("FPS: %.2f",1.0/dt),0,0);
	kit_draw_text(ctx,KIT_WHITE,elf_tpf("FPS: %.2f",1.0/dt),0,1);

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
	// elf_gsetx_cfn(&elf.R,"elf.kit.get_mouse_pos",elf_kit_get_mouse_pos);
	// elf_gsetx_cfn(&elf.R,"elf.kit.is_key_down",elf_kit_key_down);
	// elf_gsetx_cfn(&elf.R,"elf.kit.is_key_pressed",elf_kit_key_pressed);
	// elf_gsetx_cfn(&elf.R,"elf.kit.is_key_released",elf_kit_key_released);
	elf_gsetx_cfn(&elf.R,"elf.kit.create",elf_kit_create);
	elf_gsetx_cfn(&elf.R,"elf.kit.step",elf_kit_step);
	elf_gsetx_cfn(&elf.R,"elf.kit.draw_rect",elf_kit_draw_rect);
	elf_gsetx_cfn(&elf.R,"elf.kit.draw_text",elf_kit_draw_text);
	elf_gsetx_cfn(&elf.R,"elf.kit.get_delta_time",elf_kit_get_delta_time);
	elf_gsetx_cfn(&elf.R,"elf.kit.create_image",elf_kit_create_image);
	elf_gsetx_cfn(&elf.R,"elf.kit.load_image",elf_kit_load_image);
	elf_gsetx_cfn(&elf.R,"elf.kit.draw_image",elf_kit_draw_image);

	elf_add_cfn(&elf.R,core_lib_load_file);
	elf_add_nil(&elf.R);
	elf_new_string(&elf.R,"launch.elf");
	elf_call_function(&elf.R,2,0);
}