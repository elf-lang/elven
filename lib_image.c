
int lib_gfx_make_image(elf_State *S);
int lib_gfx_load_image(elf_State *S);
int lib_gfx_save_image(elf_State *S);
int lib_gfx_repl_image_colors(elf_State *S);
int lib_gfx_make_image_mask(elf_State *S);
int lib_gfx_image_get(elf_State *S);

static elf_CBinding lib_image[]={
	{"load_image",lib_gfx_load_image},
	{"save_image",lib_gfx_save_image},
	{"get",lib_gfx_image_get},
	{"repl_image_colors",lib_gfx_repl_image_colors},
	{"make_image_mask",lib_gfx_make_image_mask},
};

// void lib_image_include(elf_State *S) {
// 	elf_table_set(S->M->globals,VSTR(elf_new_string(S,"elf.gfx.load_image")),VCFN(lib_gfx_load_image));
// 	elf_table_set(S->M->globals,VSTR(elf_new_string(S,"elf.gfx.save_image")),VCFN(lib_gfx_save_image));
// 	elf_table_set(S->M->globals,VSTR(elf_new_string(S,"elf.gfx.image_replace_colors")),VCFN(lib_gfx_repl_image_colors));
// 	elf_table_set(S->M->globals,VSTR(elf_new_string(S,"elf.gfx.image_mask")),VCFN(lib_gfx_make_image_mask));
// }

int lib_gfx_make_image(elf_State *S) {
	int w = elf_get_int(S,0);
	int h = elf_get_int(S,1);
	kit_Image *img = kit_create_image(w,h);

	elf_Table *tab = elf_new_table(S);
	elf_tsets_int(tab,elf_new_string(S,"@ptr"),(elf_Int)(img));
	elf_tsets_int(tab,elf_new_string(S,"w"),w);
	elf_tsets_int(tab,elf_new_string(S,"h"),h);
	elf_add_table(S,tab);
	return 1;
}

int lib_gfx_save_image(elf_State *S) {
	char *name=elf_get_text(S,0);
	elf_Table *tab=elf_get_table(S,1);
	kit_Image *img=(kit_Image *)elf_table_get(tab,VSTR(elf_new_string(S,"@ptr"))).x_int;
	int res=stbi_write_png(name,img->w,img->h,4,img->pixels,img->w*4);
	elf_push_int(S,res);
	return 1;
}

int lib_gfx_load_image(elf_State *S) {
	char *name = elf_get_text(S,0);
	kit_Image *img=0;
	if (name){
		// todo:
		int n=0;
		int w,h;
		void *data=stbi_load(name,&w,&h,&n,4);
		if(data){
			img=calloc(1,sizeof(*img));
			img->w=w;
			img->h=h;
			img->pixels=data;
			// todo:
			for(int i=0;i<img->w*img->h;i++){
				int r=img->pixels[i].r;
				img->pixels[i].r=img->pixels[i].b;
				img->pixels[i].b=r;
			}
		}
	}
	if(img){
		elf_Table *tab = elf_new_table(S);
		elf_tsets_int(tab,elf_new_string(S,"@ptr"),(elf_Int)(img));
		elf_tsets_int(tab,elf_new_string(S,"width"),!img ? 0 : img->w);
		elf_tsets_int(tab,elf_new_string(S,"height"),!img ? 0 : img->h);
		elf_add_table(S,tab);
	}else{
		elf_push_nil(S);
	}
	return 1;
}

int lib_gfx_image_get(elf_State *S) {
	elf_Table *tab = elf_get_table(S,0);

	kit_Image *img = (kit_Image *) elf_table_get(tab
	, VSTR(elf_new_string(S,"@ptr"))).x_int;
	int x = elf_get_int(S,1);
	int y = elf_get_int(S,2);

	kit_Color color = img->pixels[img->w*x+y];
	elf_Table *color_t = elf_new_table(S);
	elf_table_set(color_t,VSTR(elf_new_string(S,"r")),VINT(color.r));
	elf_table_set(color_t,VSTR(elf_new_string(S,"g")),VINT(color.g));
	elf_table_set(color_t,VSTR(elf_new_string(S,"b")),VINT(color.b));
	elf_table_set(color_t,VSTR(elf_new_string(S,"a")),VINT(color.a));
	elf_add_table(S,color_t);
	return 1;
}

int lib_gfx_repl_image_colors(elf_State *S) {

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

	elf_push_int(S,num_occurrences);
	return 1;
}

int lib_gfx_make_image_mask(elf_State *S) {
	int i = 0;
	elf_Table *tab = elf_get_table(S,i++);
	kit_Image *img = (kit_Image *) elf_table_get(tab
	, VSTR(elf_new_string(S,"@ptr"))).x_int;

	int num_occurrences = 0;
	kit_Color *pixels = img->pixels;
	for (int i = 0; i < img->w * img->h; i ++) {
		if (pixels[i].a != 0) {
			pixels[i].r = 0;
			pixels[i].g = 0;
			pixels[i].b = 0;
			pixels[i].a = 255;
			num_occurrences ++;
		}
	}

	elf_push_int(S,num_occurrences);
	return 1;
}
