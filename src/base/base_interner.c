

APIFUNC
Interner new_interner_from_arena(Stack arena, u32 sparse_size) {
	if (sparse_size == 0) sparse_size = 1024;

	ASSERT(ISPOW2(sparse_size));

	Interner ir = stack_push_zero(arena, sizeof(*ir) + sparse_size * sizeof(*ir->sparse));
	ir->arena       = arena;
	ir->sparse_size = sparse_size;
	ir->sparse_fill = 0;
	return ir;
}

APIFUNC
Interner new_interner(u32 sparse_size) {
	Stack arena = stack_new(0, "interner arena");
	return new_interner_from_arena(arena, sparse_size);
}

APIFUNC
Atom *commit_atom_if_missing(Interner ir, Atom *atom, u32 user) {

	Stack arena = ir->arena;
	Atom **sparse = ir->sparse;
	u32 mask = ir->sparse_size - 1;

	ASSERT(atom == (Atom *) stack_top(arena));

	hash32 hash = hash_text(atom->text, atom->size);
	assert(hash != 0);

	Atom *tail = 0;
	for (u32 miss = 0; miss < ir->sparse_size; ++ miss) {
		tail = sparse[hash + miss & mask];
		if (!tail) {
			sparse[hash + miss & mask] = atom;

			stack_push(arena, sizeof(*atom) + atom->size + 1);

			++ ir->sparse_fill;
			break;
		}
		else if (tail->hash == hash && tail->size == atom->size) {
			if (memory_match(tail->text, atom->text, atom->size)) {
				break;
			}
		}
	}
	return tail;
}

APIFUNC
Atom *intern_string(Interner ir, String string, u32 user) {

	Stack arena = ir->arena;
	Atom **sparse = ir->sparse;
	u32 mask = ir->sparse_size - 1;

	hash32 hash = hash_text(string.text, string.size);
	assert(hash != 0);

	Atom *tail = 0;
	for (u32 miss = 0; miss < ir->sparse_size; ++ miss) {
		tail = sparse[hash + miss & mask];
		if (!tail) {

			++ ir->sparse_fill;

			stack_align(arena, 4);
			tail = stack_push(arena, sizeof(*tail) + string.size + 1);
			tail->hash = hash;
			tail->size = string.size;
			tail->user = user;
			copy_memory(tail->text, string.text, string.size);
			tail->text[string.size] = 0;

			sparse[hash + miss & mask] = tail;
			break;
		}
		else if (tail->hash == hash && tail->size == string.size) {
			if (memory_match(tail->text, string.text, string.size)) {
				break;
			}
		}
	}
	return tail;
}
