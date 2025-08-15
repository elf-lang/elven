
int InstallFont(R_Renderer *rend, int id, char *name, int size);
int UninstallFont(R_Renderer *rend, int id);

void D_DrawText(R_Renderer *rend, f32 x, f32 y, const char *text);
f32 D_MeasureText(const char *text);
