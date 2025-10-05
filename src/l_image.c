





static inline Color get_color(Image *img, int x, int y) {
	return img->data[x + y * img->reso.x];
}












ELF_FUNCTION(L_GetImageRegionAsString) {
	Image *img = load_image(S, 1);
	int x0 = elf_loadint(S, 2);
	int y0 = elf_loadint(S, 3);
	int w  = elf_loadint(S, 4);
	int h  = elf_loadint(S, 5);
	int x1 = x0 + w;
	int y1 = y0 + h;


	// 4 bytes per color, 2 hex bytes per color byte
	int l = w * h * 4 * 2;

	char *buf = malloc(l);

	char *c = buf;

	for (int y = y0; y < y1; ++ y) {
		for (int x = x0; x < x1; ++ x) {
			Color color = get_color(img, x, y);

			#define HEXDIGIT(i) ("0123456789ABCDEF"[(i) & 15])

			c[0] = HEXDIGIT(color.r >> 4);
			c[1] = HEXDIGIT(color.r);

			c[2] = HEXDIGIT(color.g >> 4);
			c[3] = HEXDIGIT(color.g);

			c[4] = HEXDIGIT(color.b >> 4);
			c[5] = HEXDIGIT(color.b);

			c[6] = HEXDIGIT(color.a >> 4);
			c[7] = HEXDIGIT(color.a);

			c += 8;
		}
	}

	elf_pushtext2(S, buf, l);

	free(buf);
	return 1;
}












ELF_FUNCTION(L_GetPixelPacked) {
	Image *img = load_image(S, 1);
	int x = elf_loadint(S, 2);
	int y = elf_loadint(S, 3);
	Color color = get_color(img, x, y);
	elf_pushint(S, COLOR_PACK(color));
	return 1;
}












ELF_FUNCTION(L_GetPixelAlpha) {
	Image *img = load_image(S, 1);
	int x = elf_loadint(S, 2);
	int y = elf_loadint(S, 3);
	Color color = get_color(img, x, y);
	elf_pushint(S, color.a);
	return 1;
}













ELF_FUNCTION(L_SetPixel)
{
	Image *img = load_image(S, 1);
	int x = elf_loadint(S, 2);
	int y = elf_loadint(S, 3);
	Color color = _get_color_args2(S, 4, nargs);

	img->data[x + y * img->reso.x] = color;
	return 0;
}












ELF_FUNCTION(L_SaveImage)
{
	Image *img = load_image(S, 1);
	const char *name = elf_loadtext(S, 2);

	stbi_write_bmp(name, img->reso.x,img->reso.y,4,img->data);
	return 0;
}














ELF_FUNCTION(L_NewImage)
{
	int xreso = elf_loadint(S, 1);
	int yreso = elf_loadint(S, 2);

	// todo: we're not even doing anything with this
	Color color = BLACK;
	_get_color_args3(S, 3, nargs, &color);

	Image *image;

	int size = sizeof(*image) + xreso * yreso * sizeof(*image->data);

	image = malloc(size);

	image->reso.x = xreso;
	image->reso.y = yreso;

	elf_pushsys(S, (elf_Handle) image);
	return 1;
}












ELF_FUNCTION(L_CopyImageRegion)
{
	int nextarg = 1;
	Image *dst_i = (Image *) elf_loadsys(S, nextarg ++);

	vec2i dst_v;
	nextarg += _load_vec2i(S, nextarg, nargs, &dst_v);

	Image *src_i = (Image *) elf_loadsys(S, nextarg ++);

	iRect src_r = { 0, 0, src_i->reso.x, src_i->reso.y };
	if (nargs - nextarg >= 4) {
		nextarg += _load_irect(S, nextarg, nargs, &src_r);
	}

	ASSERT(src_r.x>=0);
	ASSERT(src_r.y>=0);

	ASSERT(dst_v.x>=0);
	ASSERT(dst_v.y>=0);

	ASSERT(src_r.x+src_r.w<=src_i->reso.x);
	ASSERT(src_r.y+src_r.h<=src_i->reso.y);

	ASSERT(dst_v.x+src_r.w<=dst_i->reso.x);
	ASSERT(dst_v.y+src_r.h<=dst_i->reso.y);

	for (i32 y = 0; y < src_r.h; ++ y) {
		Color *dst = dst_i->data + dst_v.x + (dst_v.y + y) * dst_i->reso.x;
		Color *src = src_i->data + src_r.x + (src_r.y + y) * src_i->reso.x;
		memcpy(dst, src, sizeof(*src) * src_r.w);
	}

	return 0;
}










ELF_FUNCTION(L_GetImageReso) {
	Image *img = load_image(S, 1);
	_push_vec2i(S, img->reso);
	return 1;
}











ELF_FUNCTION(L_LoadImageInfo) {
	const char *name = elf_loadtext(S, 1);

	vec2i reso;
	int channels;
	int noerror = stbi_info(name, &reso.x, &reso.y, &channels);

	if (noerror) {
		elf_pushtab(S);

		elf_pushtext(S, "width");
		elf_pushint(S, reso.x);
		elf_setfield(S);

		elf_pushtext(S, "height");
		elf_pushint(S, reso.y);
		elf_setfield(S);

		elf_pushtext(S, "channels");
		elf_pushint(S, channels);
		elf_setfield(S);
	}
	else {
		elf_pushnil(S);
	}
	return 1;
}











ELF_FUNCTION(L_FreeImage)
{
	Image *img = load_image(S, 1);
	free(img);
	return 0;
}











ELF_FUNCTION(L_LoadImage)
{
	const char *name = elf_loadtext(S, 1);
	ASSERT(name);

	vec2i reso;
	i32   nch;
	i32   rch = 4;

	unsigned char *data = stbi_load(name, &reso.x, &reso.y, &nch, rch);
	if (data) {
		Image *image;
		int data_size = reso.x * reso.y * sizeof(*image->data);

		image = malloc(sizeof(*image) + data_size);
		image->reso = reso;
		memcpy(image->data, data, data_size);

		free(data);

		elf_pushsys(S, (elf_Handle) image);
		elf_pushint(S, reso.x);
		elf_pushint(S, reso.y);
		return 3;
	}
	else {
		elf_pushnil(S);
		return 1;
	}
}












elf_Binding l_image[] = {
	{ "NewImage"               , L_NewImage                },
	{ "LoadImageInfo"          , L_LoadImageInfo           },
	{ "LoadImage"              , L_LoadImage               },
	{ "SaveImage"              , L_SaveImage               },
	{ "SetPixel"               , L_SetPixel                },
	{ "GetPixelAlpha"          , L_GetPixelAlpha           },
	{ "GetPixelPacked"         , L_GetPixelPacked          },
	{ "CopyImageRegion"        , L_CopyImageRegion         },
	{ "GetImageReso"           , L_GetImageReso            },
	{ "FreeImage"              , L_FreeImage               },
	{ "GetImageRegionAsString" , L_GetImageRegionAsString  },
};


