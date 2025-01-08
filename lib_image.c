/* todo: switch to using stb's */

int lib_image_create_image(elState *S);
int lib_image_create_image(elState *S);
int lib_image_load_image(elState *S);


void lib_gfx_image_init(elf_State *S) {
	elf_table_set(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.load_image"))
	,	VCFN(lib_image_load_image));
}

int lib_image_create_image(elState *S) {
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


int lib_image_load_image(elState *S) {
	char *name = elf_get_txt(S,0);
	kit_Image *img = kit_load_image_file(name);
	elf_Table *tab = elf_new_table(S);
	elf_tsets_int(tab,elf_new_string(S,"handle"),(elf_Int)(img));
	elf_tsets_int(tab,elf_new_string(S,"width"),!img ? 0 : img->w);
	elf_tsets_int(tab,elf_new_string(S,"height"),!img ? 0 : img->h);
	elf_add_tab(S,tab);
	return 1;
}