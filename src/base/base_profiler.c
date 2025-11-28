
typedef struct  {
	Seconds     init_time;
	Seconds     last_proc_time;
	// f64          cpu_usage_pct;
	Seconds     exec_time;
	_time_field *sparse;
	u32          sparse_size;
} _time_table;

typedef struct  {
	_time_field  *field;
	Seconds      time;
} _time_entry;

global thread_decl _time_table _tt;
global thread_decl _time_entry _time_stack[16];
global thread_decl i32         _time_index;
global thread_decl b32         _time_profiler_active;

APIFUNC
void prof_init(Stack stack) {
	_time_profiler_active = 1;
	_tt.sparse_size = 4096;
	_tt.sparse = stack_push_zero(stack, _tt.sparse_size * sizeof(*_tt.sparse));
}

enum {
	FREE_KEY_CODE = 0,
};

typedef struct {
	_time_field *field;
	b32          found;
} _Find;

FILEFUNC
_Find _find_field(_time_key key) {
	ASSERT(key.code != 0);
	b32 found = false;
	u32 mask = _tt.sparse_size - 1;
	u32 hash = (u32) (uintptr_t) key.code;
	u32 miss, tail;
	for (miss = 0; miss < _tt.sparse_size; ++ miss) {
		tail = hash + miss & mask;
		if (_tt.sparse[tail].key.code == FREE_KEY_CODE) {
			_tt.sparse[tail].key = key;
			goto _esc;
		}
		else if (_tt.sparse[tail].key.code == key.code) {
			found = true;
			goto _esc;
		}
	}
	_err:
	ASSERT(!"");
	_esc:
	_Find find = { .field = & _tt.sparse[tail], .found = found };
	return find;
}

APIFUNC
void _prof_push_time_entry(_time_key key, const char *name) {
	if (!_time_profiler_active) {
		return;
	}
	ASSERT(_time_index < 16);
	_Find find = _find_field(key);
	// idk we could just store this regardless
	if (!find.found) {
		find.field->name = name;
		find.field->nest = _time_index;
	}
	find.field->freq ++;
	_time_stack[_time_index].field = find.field;
	_time_stack[_time_index].time = platform_get_performance_time();
	++ _time_index;
}

APIFUNC
void _prof_pop_time_entry() {
	if (!_time_profiler_active) {
		return;
	}
	ASSERT(_time_index > 0);
	Seconds time = platform_get_performance_time();
	_time_entry entry = _time_stack[-- _time_index];
	entry.field->time.seconds += time.seconds - entry.time.seconds;
}

APIFUNC
_Time_fields *prof_gather(Stack stack) {
	_Time_fields *gather = stack_push_zero(stack, sizeof(*gather));
	for (u32 i = 0; i < _tt.sparse_size; ++ i) {
		_time_field field = _tt.sparse[i];
		if (field.key.code != FREE_KEY_CODE) {
			stack_push_copy(stack, sizeof(field), &field);
			++ gather->nfields;
		}
	}
	return gather;
}

APIFUNC
u32 prof_dump(_Time_fields *fields) {
	// printf("profiler time table: %3.2f%% CPU\n", _tt.cpu_usage_pct * 100);
	for (u32 i = 0; i < fields->nfields; ++ i) {
		_time_field field = fields->fields[i];
		u32 indent = field.nest * 2;
		printf("%*s%-*s x%-5i tot: %8.3fms   avg: %8.3fms\n"
		, indent, ""
		, 32 - indent
		, field.name
		, field.freq
		, field.time.seconds * 1000
		, field.time.seconds * 1000 / field.freq);
	}
	return fields->nfields;
}