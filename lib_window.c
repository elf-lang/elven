// Todo: my own windows layer apparently...

int window__poll(elf_State *S);
int window__new(elf_State *S);

void lib_gfx_window_init(elf_State *S) {
	elf_Table *meta=elf_new_table(S);

	elf_table_set(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.Window"))
	, 	VTAB(meta));

	elf_table_set(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.Window.DOWN"))
	, 	VINT(KIT_INPUT_DOWN));

	elf_table_set(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.Window.PRESSED"))
	, 	VINT(KIT_INPUT_PRESSED));

	elf_table_set(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.Window.RELEASED"))
	, 	VINT(KIT_INPUT_RELEASED));

	elf_table_set(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.Window.RELEASED"))
	, 	VINT(KIT_INPUT_RELEASED));

	lib_gfx_draw_apply(S,meta);

	elf_table_set(meta
	,	VSTR(elf_new_string(S,"__new"))
	, 	VCFN(window__new));

	elf_table_set(meta
	,	VSTR(elf_new_string(S,"poll"))
	, 	VCFN(window__poll));
}

int window__new(elf_State *S) {
	// ASSERT(elf_get_this(S)->type==GC_TAB);

	elf_Table *_this=(elf_Table*)elf_get_this(S);

	elf_Table *_meta=elf_table_get(S->M->globals
	,	VSTR(elf_new_string(S,"elf.gfx.Window"))).x_tab;
	ASSERT(_this->obj.meta==_meta);

	int i = 0;
	char *name = elf_get_txt(S,i++);
	int base_res_x = elf_get_int(S,i++);
	int base_res_y = elf_get_int(S,i++);
	int window_scale = elf_get_int(S,i++);

	int flags = 0; // KIT_FPS144;
	if(window_scale==0)window_scale=1;
	if(window_scale==2)flags |= KIT_SCALE2X; else
	if(window_scale==3)flags |= KIT_SCALE3X; else
	if(window_scale==4)flags |= KIT_SCALE4X;

	kit_Context *c = kit_create(name, base_res_x, base_res_y, flags);

	elf_table_set(_this
	,	VSTR(elf_new_string(S,"@ptr"))
	,	VINT((elf_i64)c));

	// call poll once
	elf_add_cfn(S,window__poll);
	elf_add_this(S);
	elf_call(S,1,0);

	elf_add_this(S);
	return 1;
}

int window__poll(elf_State *S) {
	elf_Table *_this;
	_this = (elf_Table *) elf_get_this(S);

	kit_Context *ctx = get_ctx(S);
	ctx->mouse_wheel_y = 0;
	ctx->mouse_wheel_y = 0;

	double time;
	int x,y;
	int b;

	b=kit_step(ctx, &time);

	kit_mouse_pos(ctx,&x,&y);

#define SET(NAME,VALUE) elf_table_set(_this,VSTR(elf_new_string(S,NAME)),VALUE)
#define SET_INT(NAME,VALUE) SET(NAME,VINT(VALUE))
#define SET_NUM(NAME,VALUE) SET(NAME,VNUM(VALUE))
#define SET_TAB(NAME,VALUE) SET(NAME,VTAB(VALUE))

	// todo: reuse the old table?
	elf_Table *keys = elf_new_table(S);
	for(int i=0; i<COUNTOF(ctx->key_state); i++){
		elf_table_set(keys
		, 	VINT(i)
		,	VINT(ctx->key_state[i]));
	}
	elf_Table *mouse = elf_new_table(S);
	for(int i=0; i<COUNTOF(ctx->mouse_state); i++){
		elf_table_set(mouse
		, 	VINT(i)
		,	VINT(ctx->mouse_state[i]));
	}

	SET_INT("res_x",ctx->screen->w);
	SET_INT("res_y",ctx->screen->h);
	SET_INT("mouse_x",x);
	SET_INT("mouse_y",y);
	SET_INT("mouse_wheel_x",ctx->mouse_wheel_x);
	SET_INT("mouse_wheel_y",ctx->mouse_wheel_y);
	SET_NUM("time",time);
	SET_TAB("keys",keys);
	SET_TAB("mouse",mouse);


	kit_clear(ctx,KIT_BLACK);

	elf_add_int(S,b);
	return 1;
}