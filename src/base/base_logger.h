
typedef enum {
	LOG_DEBUG = 0,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_SUCCESS,
	LOG_TYPE_COUNT,
} LOG_TYPE;

typedef struct {
	const char *file;
	int line;
} SourceLoc;


void base_logger(LOG_TYPE type, const char *custom_tag, SourceLoc loc, const char *message);

#define SOURCE_LOC_HERE ((SourceLoc){ __FILE__, __LINE__ })

#define XLOG(type, msg, ...) base_logger(type, 0, SOURCE_LOC_HERE, temp_format_(msg, __VA_ARGS__))
#define TRACELOG(msg, ...) XLOG(LOG_DEBUG, msg, __VA_ARGS__)
#define ERRORLOG(msg, ...) XLOG(LOG_ERROR, msg, __VA_ARGS__)
#define WARN(msg, ...)     XLOG(LOG_WARNING, msg, __VA_ARGS__)
#define INFO(msg, ...)     XLOG(LOG_INFO, msg, __VA_ARGS__)
#define SUCCESSLOG(msg, ...) XLOG(LOG_SUCCESS, msg, __VA_ARGS__)

