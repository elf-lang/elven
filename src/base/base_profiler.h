


typedef struct {
	void *code;
} _time_key;

#define FRAME_SCOPE(BEGIN, END) for (u32 _once = 1; (_once) && ((BEGIN), 1); (END), (_once = 0))
#define TIME_FRAME_(ID, LABEL) FRAME_SCOPE(_prof_push_time_entry((_time_key){ID}, LABEL), _prof_pop_time_entry())
#define TIME_FRAME_DECL(NAME, LABEL) static u32 NAME; TIME_FRAME_(&NAME, LABEL)
#define TIME_FRAME(LABEL) TIME_FRAME_DECL(CONCAT(_time_,__COUNTER__), LABEL)


typedef struct {
	_time_key    key;
	Seconds    time;
	const char *name;
	u32         nest;
	u32         freq;
} _time_field;

typedef struct {
	u32         nfields;
	_time_field  fields[];
} _Time_fields;


_Time_fields *prof_gather(Stack stack);

void prof_init(Stack stack);
u32 prof_dump(_Time_fields *);

void _prof_push_time_entry(_time_key key, const char *name);
void _prof_pop_time_entry();
