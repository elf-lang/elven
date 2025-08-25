


// todo: if it's just one value then grayscale
// todo: color modes for HSV!
static inline Color _get_color_args2(elf_State *S, int index, int nargs) {
	nargs -= index;

	Color color = {0,0,0,255};
	if (nargs >= 3) {
		color.r = (u8) elf_loadint(S, index + 0);
		color.g = (u8) elf_loadint(S, index + 1);
		color.b = (u8) elf_loadint(S, index + 2);
		if (nargs >= 4) {
			color.a = (u8) elf_loadint(S, index + 3);
		}
	}
	return color;
}


static inline Color _get_color_arg(elf_State *S, int nargs) {
	return _get_color_args2(S, 1, nargs);
}


