// arbitrarily large matrix lib!
// todo: implement!


typedef struct {
	int w, h;
	f32 rows[];
} D_Matrix;


ELF_FUNCTION(L_NewMatrix) {
	int w = elf_loadint(S, 1);
	int h = elf_loadint(S, 2);
	// todo: GC!
	D_Matrix *matrix = malloc(sizeof(*matrix) + w * h * sizeof(*matrix->rows));
	return 1;
}


static Matrix ScaleMatrix(vec3 v) {
	Matrix c = {
		v.x,   0,   0, 0,
		0, v.y,   0, 0,
		0,   0, v.z, 0,
		0,   0,   0, 1,
	};
	return c;
}



static Matrix RotationMatrix(f32 r) {
	f32 co = cosf(r);
	f32 si = sinf(r);
	Matrix c = {
		co,  si, 0, 0,
		si, -co, 0, 0,
		0,    0, 1, 0,
		0,    0, 0, 1,
	};
	return c;
}



static Matrix MultiplyMatrices(Matrix a, Matrix b) {
	Matrix c;
	for (int i = 0; i < 4; i ++) {
		for (int j = 0; j < 4; j ++) {
			c.rows[i].xyzw[j] =
			a.rows[i].xyzw[0] * b.rows[0].xyzw[j] +
			a.rows[i].xyzw[1] * b.rows[1].xyzw[j] +
			a.rows[i].xyzw[2] * b.rows[2].xyzw[j] +
			a.rows[i].xyzw[3] * b.rows[3].xyzw[j];
		}
	}
	return c;
}



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


