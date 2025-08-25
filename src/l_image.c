typedef struct {
	vec2i reso;
	Color data[];
} Image;


ELF_FUNCTION(L_SetPixel)
{
	Image *img = (Image *) elf_loadsys(S, 1);
	int x = elf_loadint(S, 2);
	int y = elf_loadint(S, 3);
	Color color = _get_color_args2(S, 4, nargs);

	img->data[x + y * img->reso.x] = color;
	return 0;
}


ELF_FUNCTION(L_SaveImage)
{
	Image *img = (Image *) elf_loadsys(S, 1);
	const char *name = elf_loadtext(S, 2);

	stbi_write_bmp(name, img->reso.x,img->reso.y,4,img->data);
	return 0;
}


ELF_FUNCTION(L_LoadImage)
{
	const char *name = elf_loadtext(S, 1);

	vec2i reso;
	i32   nch;
	i32   rch = 4;
	unsigned char *data = stbi_load(name, &reso.x, &reso.y, &nch, rch);

	Image *image;
	int data_size = reso.x * reso.y * sizeof(*image->data);

	image = malloc(sizeof(*image) + data_size);
	image->reso = reso;
	memcpy(image->data, data, data_size);

	free(data);

	elf_pushsys(S, (elf_Handle) image);
	return 1;
}



elf_Binding l_image[] = {
	{ "LoadImage"   , L_LoadImage   },
	{ "SaveImage"   , L_SaveImage   },
	{ "SetPixel"    , L_SetPixel    },
};