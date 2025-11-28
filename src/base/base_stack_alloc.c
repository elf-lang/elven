
void stack_end(Stack stack) {
	_aligned_free(stack);
}

// todo: virtual alloc!
Stack stack_new(i64 max, const char *name) {
	if (max == 0) max = MEGABYTES(64);

	Stack stack = _aligned_malloc(sizeof(*stack) + max, 16);
	stack->min = 0;
	stack->max = max;
	stack->name = name;
	return stack;
}

void *stack_align(Stack stack, u32 align) {
	require(align != 0, "align must be non-zero");
	uintptr_t mask = align - 1;
	require((align & mask) == 0, "align must me power of 2");
	stack->min = (((uintptr_t) stack->mem + (stack->min + mask)) & ~mask) - (uintptr_t) stack->mem;
	ASSERT(((uintptr_t) (stack->mem + stack->min) & mask) == 0);
	return stack->mem + stack->min;
}

void stack_reset(Stack stack) {
	stack->min = 0;
}

void *stack_top(Stack stack) {
	return stack->mem + stack->min;
}

void *stack_bot(Stack stack) {
	return stack->mem;
}

i64 stack_used(Stack stack) {
	return stack->min;
}

void *stack_pop(Stack stack, i64 size) {
	ASSERT(stack->min + size <= stack->max);
	ASSERT(stack->min + size >= 0);
	stack->min -= size;
	void *ptr = stack->mem + stack->min;
	return ptr;
}

void *stack_push_d(Stack stack, i64 size, const char *name) {
	if (stack->max == 0) {
		ERRORLOG("\n\tstack '%s' has zero capacity", stack->name);
	}
	if (stack->min + size > stack->max) {
		ERRORLOG("\n\tstack '%s' ran out of memory when pushing size: %s""\n\t\t capacity: %s""\n\t\t usage: %s"
		,		stack->name
		,		format_size_decomposed_complete(size)
		,		format_size_decomposed_complete(stack->max)
		,		format_size_decomposed_complete(stack->min));
	}
	ASSERT(stack->min + size <= stack->max);
	ASSERT(stack->min + size >= 0);
	stack->min += MIN(size, 0);
	void *ptr = stack->mem + stack->min;
	stack->min += MAX(size, 0);
	return ptr;
}

void *stack_push_fill_d(Stack stack, i64 size, u32 fill, const char *name) {
	void *result = stack_push_d(stack, size, name);
	fill_memory(result, fill, size);
	return result;
}

void *stack_push_zero_d(Stack stack, i64 size, const char *name) {
	void *ptr = stack_push_d(stack, size, name);
	fill_memory(ptr, 0, size);
	return ptr;
}

void *stack_push_copy_d(Stack stack, i64 size, const void *data, const char *name) {
	void *dest = stack_push_d(stack, size, name);
	memcpy(dest, data, size);
	return dest;
}
