


void requirement_failed(const char *file, int line, const char *req, const char *notes);

#define require(req, notes) if (!(req)) requirement_failed(__FILE__, __LINE__, #req, notes)
