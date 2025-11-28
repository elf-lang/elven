//
// ALL STRINGS ARE NULL TERMINATED
// Todo, remove const qualifier from string data, who tf cares
//
#define BASE_STRING_FIELDS() struct { union { const char *text, *data, *chars; }; u32 size; }

typedef struct {
	BASE_STRING_FIELDS();
} String;

#define LIT(lt) ((String) { .text = (lt), .size = sizeof(lt) - 1 })

PORTABLEFUNC
String string_from_data(const void *text, u32 size) {
	String result = {};
	result.text = (char *) text;
	result.size = size;
	return result;
}

String push_append_string(Stack stack, String string);

String push_append_string(Stack stack, String string);
String push_append_strings(Stack stack, String *strings, u32 num_strings, String separator);

hash32 string_hash(String a);
b32 string_match(String a, String b);
i32 string_find_exact_match(String str, String substr);
String string_slice(String str, i32 index, i32 size);

__attribute__((format(printf, 2, 3)))
String push_append_formatted(Stack stack, const char *format, ...);

String push_formatted_string_v(Stack stack, const char *format, va_list vargs);
String push_formatted(Stack stack, const char *format, ...) __attribute__((format(printf, 2, 3)));
String push_string_wrap(Stack stack, const char *text);
String push_string_copy(Stack stack, String str);

// Todo, literally remove all these other ones...

__attribute__((format(printf, 3, 4)))
char *stack_format_l(Stack stack, u32 *length, const char *format, ...);
char *stack_format_v(Stack stack, const char *format, u32 *length, va_list vargs);
char *stack_format(Stack stack, const char *format, ...) __attribute__((format(printf, 2, 3)));