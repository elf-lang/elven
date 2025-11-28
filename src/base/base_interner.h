
typedef struct Atom Atom;
struct Atom {
	hash32  hash;
	u16     size;
	u16     user;
	char    text[];
};

typedef struct {
	Stack  arena;
	u32    sparse_size;
	u32    sparse_fill;
	Atom  *sparse[];
} Interner_;

typedef Interner_ *Interner;

#define INTERN_LITERAL(ir, s, user) intern_string(ir, LIT(s), user)

Interner new_interner_from_arena(Stack arena, u32 sparse_size);

// The atom must be at the very top, stack_top() == atom.
// The hash isn't required.
Atom *commit_atom_if_missing(Interner ir, Atom *atom, u32 user);
Interner new_interner(u32 sparse_size);
Atom *intern_string(Interner ir, String string, u32 user);

// Todo! remove this version
PORTABLEFUNC
Atom *intern_string_old(Interner ir, const char *text, u32 size, u32 user) {
	return intern_string(ir, string_from_data(text, size), user);
}


