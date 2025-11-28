

void requirement_failed(const char *file, int line, const char *req, const char *msg) {
	base_logger(LOG_ERROR, "requirement error", (SourceLoc){file, line}, temp_format_("requirement failed: %s, %s", req, msg));
	__debugbreak();
	exit(1);
}