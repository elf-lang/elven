/* todo: switch to using stb's */

int lib_image_create_image(elState *S);
int lib_image_load_image(elState *S);
int lib_image_replace_colors(elState *S);


void lib_gfx_image_init(elf_State *S) {
	elf_table_set(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.load_image"))
	,	VCFN(lib_image_load_image));

	elf_table_set(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.image_replace_colors"))
	,	VCFN(lib_image_replace_colors));
}

int lib_image_create_image(elState *S) {
	int w = elf_get_int(S,0);
	int h = elf_get_int(S,1);
	kit_Image *img = kit_create_image(w,h);

	elf_Table *tab = elf_new_table(S);
	elf_tsets_int(tab,elf_new_string(S,"@ptr"),(elf_Int)(img));
	elf_tsets_int(tab,elf_new_string(S,"w"),w);
	elf_tsets_int(tab,elf_new_string(S,"h"),h);
	elf_push_table(S,tab);
	return 1;
}

int lib_image_replace_colors(elState *S) {

	int i = 0;

	elf_Table *tab = elf_get_table(S,i++);
	kit_Image *img = (kit_Image *) elf_table_get(tab
	, VSTR(elf_new_string(S,"@ptr"))).x_int;

	int src_r = elf_get_int(S,i++);
	int src_g = elf_get_int(S,i++);
	int src_b = elf_get_int(S,i++);
	int src_a = elf_get_int(S,i++);

	int dst_r = elf_get_int(S,i++);
	int dst_g = elf_get_int(S,i++);
	int dst_b = elf_get_int(S,i++);
	int dst_a = elf_get_int(S,i++);

	int num_occurrences = 0;
	kit_Color *pixels = img->pixels;
	for (int i = 0; i < img->w * img->h; i ++) {
		if (pixels[i].r == src_r && pixels[i].g == src_g && pixels[i].b == src_b && pixels[i].a == src_a) {
			pixels[i].r = dst_r;
			pixels[i].g = dst_g;
			pixels[i].b = dst_b;
			pixels[i].a = dst_a;
			num_occurrences ++;
		}
	}

	elf_push_integer(S,num_occurrences);
	return 1;
}


int lib_image_load_image(elState *S) {
	char *name = elf_get_text(S,0);
	kit_Image *img = kit_load_image_file(name);
	elf_Table *tab = elf_new_table(S);
	elf_tsets_int(tab,elf_new_string(S,"@ptr"),(elf_Int)(img));
	elf_tsets_int(tab,elf_new_string(S,"width"),!img ? 0 : img->w);
	elf_tsets_int(tab,elf_new_string(S,"height"),!img ? 0 : img->h);
	elf_push_table(S,tab);
	return 1;
}