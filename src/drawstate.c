


static vec4 MultiplyMatrixVector(Matrix m, vec4 v) {
	vec4 c;
	for (int i = 0; i < 4; i ++) {
		c.xyzw[i] =
		v.xyzw[0] * m.rows[i].xyzw[0] +
		v.xyzw[1] * m.rows[i].xyzw[1] +
		v.xyzw[2] * m.rows[i].xyzw[2] +
		v.xyzw[3] * m.rows[i].xyzw[3];
	}
	return c;
}







