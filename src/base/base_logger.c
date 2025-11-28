
// Todo, we need to check the OS to see whether ANSII codes are enabled or not,
// or alternatively have a platform console API wrapper thing...
static const char *ansii_color_codes_for_log_type[] = {
	[LOG_DEBUG]     = "",
	[LOG_INFO]      = "\033[94m",
	[LOG_WARNING]   = "\033[35m",
	[LOG_ERROR]     = "\033[91m",
	[LOG_SUCCESS]   = "\033[32m",
};
static const char *default_log_tags[] = {
	[LOG_DEBUG]     = "trace"   ,
	[LOG_INFO]      = "info"    ,
	[LOG_WARNING]   = "warning" ,
	[LOG_ERROR]     = "error"   ,
	[LOG_SUCCESS]   = "success" ,
};

void base_logger(LOG_TYPE type, const char *tag, SourceLoc loc, const char *message) {
	if (!tag) tag = default_log_tags[type];

	global u64 counter;

	printf("%s(%i) %lli %s%s%s: %s\n", loc.file, loc.line, counter
	, ansii_color_codes_for_log_type[type], tag, "\033[0m"
	, message);

	counter ++;
}