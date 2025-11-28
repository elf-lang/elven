


static inline Image *load_image(elf_State *S, int index) {
	Image *img = (Image *) elf_loadsys(S, index);
	return img;
}



// todo: move to lib helpers in elf
static inline void checkindex2(elf_State *S, int index, int min, int max) {
	if (index < min || index >= max) {
		elf_error(S, -1, "%i: index out of bounds [%i, %i]", index, min, max);
	}
}


// todo: move to lib helpers in elf
static inline void checkindex(elf_State *S, int index, int max) {
	checkindex2(S, index, 0, max);
}




static void _push_vec2i(elf_State *S, vec2i v) {
	elf_pushtab(S);

	elf_pushtext(S, "x");
	elf_pushint(S, v.x);
	elf_setfield(S);

	elf_pushtext(S, "y");
	elf_pushint(S, v.y);
	elf_setfield(S);
}



static inline int _load_vec2i(elf_State *S, int index, int nargs, vec2i *v) {
	ASSERT(nargs - index >= 2);
	v->x = elf_loadint(S, index + 0);
	v->y = elf_loadint(S, index + 1);
	return 2;
}



static inline int _load_irect(elf_State *S, int index, int nargs, rect_i32 *rect) {
	ASSERT(nargs - index >= 4);
	rect->x = elf_loadint(S, index + 0);
	rect->y = elf_loadint(S, index + 1);
	rect->w = elf_loadint(S, index + 2);
	rect->h = elf_loadint(S, index + 3);
	return 4;
}



// clamps to 255
static void _get_color_args3(elf_State *S, int index, int nargs, Color *color) {
	nargs -= index;
	if (nargs >= 3) {
		color->r = (u8) elf_loadint(S, index + 0);
		color->g = (u8) elf_loadint(S, index + 1);
		color->b = (u8) elf_loadint(S, index + 2);
		if (nargs >= 4) {
			color->a = (u8) elf_loadint(S, index + 3);
		}
	}
}



// todo: if it's just one value then grayscale
// todo: color modes for HSV!
static inline Color _get_color_args2(elf_State *S, int index, int nargs) {
	Color color = {0,0,0,255};
	_get_color_args3(S, index, nargs, &color);
	return color;
}


static inline Color _get_color_arg(elf_State *S, int nargs) {
	return _get_color_args2(S, 1, nargs);
}


