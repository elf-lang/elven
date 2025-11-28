
typedef struct TCTX TCTX;
struct TCTX {
	Stack  main_arena;
	u8     thread_name[256];
	u64    thread_name_size;
	char  *file_name;
	u64    line_number;
};

void      tctx_init_and_equip(TCTX *tctx);
TCTX     *tctx_get_equipped(void);

Stack     tctx_get_scratch();

void      tctx_set_thread_name(String name);
String    tctx_get_thread_name(void);

void      tctx_write_srcloc(char *file_name, u64 line_number);
void      tctx_read_srcloc(char **file_name, u64 *line_number);
#define tctx_write_this_srcloc() tctx_write_srcloc(__FILE__, __LINE__)


// Todo, this is deprecated
void *thread_temp_alloc(i64 reserve);
char *temp_format_v(const char *format, u32 *length, va_list vargs);
char *temp_format_(char *format, ...)
__attribute__((format(printf, 1, 2)));
