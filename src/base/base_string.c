
APIFUNC
hash32 string_hash(String a) {
	return hash_text(a.text, a.size);
}

APIFUNC
b32 string_match(String a, String b) {
	return a.size == b.size && memory_match(a.text, b.text, a.size);
}

APIFUNC
String push_append_formatted_v(Stack stack, const char *format, va_list v) {
	u32 size = _vscprintf(format, v);
	char *text = stack_push(stack, size + 1);
	stack->min --;
	vsprintf_s(text, size + 1, format, v);
	String result = { .size = size, .text = text };
	return result;
}

APIFUNC
String push_append_formatted(Stack stack, const char *format, ...) {
	va_list v;
	va_start(v, format);
	String result = push_append_formatted_v(stack, format, v);
	va_end(v);
	return result;
}


APIFUNC
String push_append_string(Stack stack, String string) {
	u32 result_size = string.size;
	char *result_text = stack_push(stack, result_size);
	copy_memory(result_text, string.text, result_size);
	String result = { .size = result_size, .text = result_text };
	return result;
}

APIFUNC
String push_append_strings(Stack stack, String *strings, u32 num_strings, String separator) {
	String result;
	result.size = 0;
	result.data = stack_top(stack);
	for (u32 i = 0; i < num_strings; ++ i) {
		if (i != 0) {
			result.size += push_append_string(stack, separator).size;
		}
		result.size += push_append_string(stack, strings[i]).size;
	}
	return result;
}

APIFUNC
String string_slice(String str, i32 index, i32 size) {
	if (index < 0) index = str.size + index;
	if (size < 0) size = str.size - index + size + 1;

	ASSERT(size >= 0 && size <= str.size);
	ASSERT(index >= 0 && index <= str.size);
	ASSERT(index + size <= str.size);
	ASSERT(index + size >= 0);
	String result;
	result.size = size;
	result.data = str.data + index;
	return result;
}

APIFUNC
i32 string_find_exact_match(String str, String substr) {
	i32 result = -1;
	for (i32 i = 0; i <= str.size - substr.size; ++ i) {
		if (memory_match(str.data + i, substr.data, substr.size)) {
			result = i;
			break;
		}
	}
	return result;
}


APIFUNC
String push_string_wrap(Stack stack, const char *text) {
	u32 result_size = strlen(text);
	char *result_text = stack_push(stack, result_size + 1);
	copy_memory(result_text, text, result_size);
	String result = { .size = result_size, .text = result_text };
	return result;
}

APIFUNC
String push_string_copy(Stack stack, String string) {
	u32 result_size = string.size;
	char *result_text = stack_push(stack, result_size + 1);
	copy_memory(result_text, string.text, result_size);
	String result = { .size = result_size, .text = result_text };
	result_text[result_size] = 0;
	return result;
}

APIFUNC
String push_formatted_string_v(Stack stack, const char *format, va_list v) {
	u32 size = _vscprintf(format, v);
	char *text = stack_push(stack, size + 1);
	vsprintf_s(text, size + 1, format, v);
	String result = { .size = size, .text = text };
	return result;
}

APIFUNC
String push_formatted(Stack stack, const char *format, ...) {
	va_list v;
	va_start(v, format);
	String result = push_formatted_string_v(stack, format, v);
	va_end(v);
	return result;
}


// Todo, remove:

// Todo, length should come first
APIFUNC
char *stack_format_v(Stack stack, const char *format, u32 *length, va_list vargs) {
	*length = _vscprintf(format, vargs);
	char *write = stack_push(stack, *length + 1);
	vsprintf_s(write, *length + 1, format, vargs);
	return write;
}


APIFUNC
char *stack_format_l(Stack stack, u32 *length, const char *format, ...) {
	va_list vargs;
	va_start(vargs, format);
	char *write = stack_format_v(stack, format, length, vargs);
	va_end(vargs);
	return write;
}

char *stack_format(Stack stack, const char *format, ...) {
	va_list vargs;
	va_start(vargs, format);
	u32 len;
	char *write = stack_format_v(stack, format, &len, vargs);
	va_end(vargs);
	return write;
}
