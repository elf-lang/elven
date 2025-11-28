
typedef struct tStack *Stack;

typedef struct tStack tStack;
struct tStack {
	const char *name;
	i64    pad;
	i64    max,min;
	u8   	 mem[];
};

STATIC_ASSERT((offsetof(tStack, mem) & 15) == 0);


#define STACK_FRAME(s) for(i64 _s_min = (s)->min; _s_min != -1; (s)->min = _s_min, _s_min = -1)

Stack stack_new(i64 max, const char *name);
void stack_end(Stack stack);
void *stack_top(Stack stack);
void *stack_bot(Stack stack);
void stack_reset(Stack stack);
void *stack_align(Stack stack, u32 align);
i64 stack_used(Stack stack);
void *stack_push_d(Stack stack, i64 size, const char *name);
void *stack_push_fill_d(Stack stack, i64 size, u32 fill, const char *name);
void *stack_push_zero_d(Stack stack, i64 size, const char *name);
void *stack_push_copy_d(Stack stack, i64 size, const void *data, const char *name);
void *stack_pop(Stack stack, i64 size);

#define stack_push_copy(stack, size, data) stack_push_copy_d(stack, size, data, __func__)
#define stack_push_zero(stack, size) stack_push_zero_d(stack, size, __func__)
#define stack_push_fill(stack, size, fill) stack_push_fill_d(stack, size, fill, __func__)
#define stack_push(stack, size) stack_push_d(stack, size, __func__)
