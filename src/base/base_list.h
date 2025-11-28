typedef struct List_Head List_Head;
struct List_Head {
	List_Head *prev;
	List_Head *next;
};

#define container_of(field_ptr, t, field) ((t *) (((u8 *) (field_ptr)) - offsetof(t, field)))

PORTABLEFUNC INLINEFUNC
void list_remove(List_Head *h, List_Head *n) {
	n->prev->next = n->next;
	n->next->prev = n->prev;
	n->prev = n->next = 0;
}

#define list_append list_add_tail
#define list_append_xx(h, n) list_add_tail((List_Head *)(h), (List_Head *)(n))
#define list_init_xx(h)   list_init((List_Head *)(h))

PORTABLEFUNC INLINEFUNC
void list_add_tail(List_Head *h, List_Head *n) {
	ASSERT(n);
	ASSERT(!n->prev);
	ASSERT(!n->next);
	ASSERT(h);
	ASSERT(h->next);
	ASSERT(h->prev);
	ASSERT(h->prev->next == h);
	ASSERT(h->next->prev == h);
	n->next = h, n->prev = h->prev, h->prev->next = n, h->prev = n;
}

PORTABLEFUNC INLINEFUNC
void list_add_head(List_Head *h, List_Head *n) {
	ASSERT(n);
	ASSERT(!n->prev);
	ASSERT(!n->next);
	ASSERT(h);
	ASSERT(h->next);
	ASSERT(h->prev);
	ASSERT(h->prev->next == h);
	ASSERT(h->next->prev == h);
	n->prev = h, n->next = h->next, h->next->prev = n, h->next = n;
}

PORTABLEFUNC INLINEFUNC
void list_init(List_Head *h) {
	h->prev = h, h->next = h;
}

PORTABLEFUNC INLINEFUNC
b32 list_empty(List_Head *h) {
	return h->prev == h;
}

// iterate over list heads
#define list_foreach_plain(h, n) for (List_Head *(n) = (h)->next; (n) && (n) != (h); (n) = (n)->next)

// inner loop declares a typed variable pointing to the structure holding the list head, it only
// iterates once
#define list_foreach_inner_loop(h, t, f, n) for (t *(n) = (t *) container_of(h, t, f); (n); (n) = 0)
// h: pointer to list head
// t: type holding the list head
// f: type field holding the list head
// n: decl name
#define list_foreach(h, t, f, n) list_foreach_plain(h, _##n) list_foreach_inner_loop(_##n, t, f, n)