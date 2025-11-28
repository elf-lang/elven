
thread_decl TCTX *tctx_thread_local;

APIFUNC
void tctx_init_and_equip(TCTX *tctx) {
	MemoryZeroStruct(tctx);
	tctx->main_arena = stack_new(0, "thread context arena");
	tctx_thread_local = tctx;
}

APIFUNC
TCTX *tctx_get_equipped(void) {
	return tctx_thread_local;
}

APIFUNC
Stack tctx_get_scratch(){
	TCTX *tctx = tctx_get_equipped();
	Stack result = tctx->main_arena;
	return result;
}

APIFUNC
void tctx_set_thread_name(String string){
	TCTX *tctx = tctx_get_equipped();
	u64 size = Min(string.size, sizeof(tctx->thread_name));
	copy_memory(tctx->thread_name, string.text, size);
	tctx->thread_name_size = size;
}

APIFUNC
String tctx_get_thread_name(void){
	TCTX *tctx = tctx_get_equipped();
	String result = string_from_data(tctx->thread_name, tctx->thread_name_size);
	return(result);
}

APIFUNC
void tctx_write_srcloc(char *file_name, u64 line_number){
	TCTX *tctx = tctx_get_equipped();
	tctx->file_name = file_name;
	tctx->line_number = line_number;
}

APIFUNC
void tctx_read_srcloc(char **file_name, u64 *line_number){
	TCTX *tctx = tctx_get_equipped();
	*file_name = tctx->file_name;
	*line_number = tctx->line_number;
}









void *thread_temp_alloc(i64 reserve) {
	// todo:
	global thread_decl char buf[1 << 10];
	global thread_decl u32 pos;
	assert(reserve <= sizeof(buf));
	if (pos + reserve > sizeof(buf)) {
		pos = 0;
	}
	void *mem = buf + pos;
	pos += reserve;
	return mem;
}

char *temp_format_v(const char *format, u32 *plength, va_list vargs) {
	u32 length = _vscprintf(format, vargs);
	char *write = thread_temp_alloc(length + 1);
	vsprintf_s(write, length + 1, format, vargs);
	if (plength) *plength = length;
	return write;
}

char *temp_format_(char *format, ...) {
	va_list vargs;
	va_start(vargs, format);
	u32 len;
	char *write = temp_format_v(format, &len, vargs);
	va_end(vargs);
	return write;
}
