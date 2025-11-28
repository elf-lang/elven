
void D_PushMatrix() {
	g_core.xform_stack[g_core.xform_index ++] = g_core.xform;
}

void D_PopMatrix() {
	g_core.xform = g_core.xform_stack[-- g_core.xform_index];
}

void D_LoadIdentity() {
	g_core.xform.rows[0] = (vec4){ 1, 0, 0, 0 };
	g_core.xform.rows[1] = (vec4){ 0, 1, 0, 0 };
	g_core.xform.rows[2] = (vec4){ 0, 0, 1, 0 };
	g_core.xform.rows[3] = (vec4){ 0, 0, 0, 1 };
}

void D_SetRotation(f32 radians) {
	f32 co = cosf(radians);
	f32 si = sinf(radians);
	g_core.xform.rows[0].x =   co;
	g_core.xform.rows[0].y =   si;
	g_core.xform.rows[1].x =   si;
	g_core.xform.rows[1].y = - co;
}

void D_SetScale(f32 x, f32 y) {
	g_core.xform.rows[0].x = x;
	g_core.xform.rows[1].y = y;
}

void D_SetOffset(f32 x, f32 y) {
	g_core.xform.rows[0].w = x;
	g_core.xform.rows[1].w = y;
}

void D_Translate(f32 x, f32 y) {
	g_core.xform.rows[0].w += x;
	g_core.xform.rows[1].w += y;
}