#include <stdio.h>
#include <stdint.h>
#include "vector.h"

Vector vector_new(size_t init_size, size_t data_size) {
	Vector res = {};
	res.data_size = data_size;
	res.allocated = init_size;
	res.used = 0;
	res.data = malloc((init_size >= 1 ? init_size : 1) * data_size);
	return res;
}
void vector_ensure_allocated(Vector* vec, size_t size) {
	if(vec->allocated < size)
		vec->data = realloc(vec->data, vec->data_size * (vec->allocated * 2 >= size ? (vec->allocated *= 2) : (vec->allocated = size)));
}
void* vector_get_ptr(Vector* vec, size_t index) {
	return vec->data + index * vec->data_size;
}
void vector_push(Vector* vec, void* data) {
	vector_insert_array(vec, vec->used, data, 1);
}
void vector_unshift(Vector* vec, void* data) {
	vector_insert_array(vec, 0, data, 1);
}
void vector_insert(Vector* vec, size_t index, void* data) {
	vector_insert_array(vec, index, data, 1);
}
void vector_push_array(Vector* vec, void* data, size_t length) {
	vector_insert_array(vec, vec->used, data, length);
}
void vector_unshift_array(Vector* vec, void* data, size_t length) {
	vector_insert_array(vec, 0, data, length);
}
void vector_insert_array(Vector* vec, size_t index, void* data, size_t length) {
	if(length == 0) return;
	size_t i = index > vec->used ? vec->used : index;
	vector_ensure_allocated(vec, vec->used + length);
	if(i < vec->used) memmove(vector_get_ptr(vec, index + length), vector_get_ptr(vec, i), (vec->used - i) * vec->data_size);
	vec->used += length;
	memcpy(vector_get_ptr(vec, index), data, length * vec->data_size);
}
void vector_pop(Vector* vec, void* data) {
	vector_remove_many(vec, vec->used - 1, 1, data);
}
void vector_shift(Vector* vec, void* data) {
	vector_remove_many(vec, 0, 1, data);
}
void vector_remove(Vector* vec, size_t index, void* data) {
	vector_remove_many(vec, index, 1, data);
}
void vector_pop_many(Vector* vec, size_t count, void* data) {
	vector_remove_many(vec, vec->used - (count > vec->used ? vec->used : count), count, data);
}
void vector_shift_many(Vector* vec, size_t count, void* data) {
	vector_remove_many(vec, 0, count, data);
}
void vector_remove_many(Vector* vec, size_t index, size_t count, void* data) {
	if(count == 0) return;
	// clamp values to avoid out-of-range
	size_t i = index >= vec->used ? vec->used - 1 : index;
	size_t c = i + count > vec->used ? vec->used - i : count;
	if(data != NULL) memcpy(data, vector_get_ptr(vec, i), c * vec->data_size);
	if(vec->used - i - c != 0) memmove(vector_get_ptr(vec, i), vector_get_ptr(vec, i + c), (vec->used - i - c) * vec->data_size);
	vec->used -= c;
}
Vector vector_splice(Vector* vec, size_t index, size_t count) {
	size_t i = index >= vec->used ? vec->used - 1 : index;
	size_t c = i + count >= vec->used ? vec->used - i : count;
	Vector res = vector_new(c, vec->data_size);
	res.used = count;
	vector_remove_many(vec, i, c, res.data);
	return res;
}
void vector_shrink(Vector* vec) {
	vec->data = realloc(vec->data, (vec->allocated = vec->used) * vec->data_size);
}
void vector_set(Vector* vec, void* data, size_t index) {
	// TODO: Logger
	if(index >= vec->used) return;
	memcpy(vector_get_ptr(vec, index), data, vec->data_size);
}
void vector_concat(Vector* vec, Vector* other) {
	vector_push_array(vec, other->data, other->used);
}
Vector vector_clone(Vector* vec) {
	Vector res = vector_new(vec->allocated, vec->data_size);
	vector_concat(&res, vec);
	return res;
}
void vector_merge(Vector* vec, Vector* other) {
	vector_concat(vec, other);
	vector_free(other);
}
void vector_free(Vector* vec) {
	free(vec->data);
	vec->allocated = 0;
	vec->used = 0;
}
bool vector_has(Vector* vec, size_t index) {
	return index < vec->used;
}
char* vector_to_string(Vector* vec, char* format) {
	Vector res = vector_new(5, sizeof(char));
	char start[1] = "[";
	char delim[2] = ", ";
	size_t delim_len = sizeof(delim) / sizeof(char);
	vector_push_array(&res, start, 1);
	for(int i = 0; i < vec->used; i++) {
		// see http://www.swig.org/Doc2.0/Varargs.html for a better explanation.
		//
		// here the intmax_t is used to make sure any printable type
		// fits in the argument. This is because of how C does arguments:
		// when a function is called in C, the compiler assumes (rightfully
		// so), that the call will keep the same arguments, of the same type
		// throughout the entire execution of the program. The reason it does
		// that is because there simply is no way to change the arguments,
		// without using inline asembler and implementation specific hacking,
		// and while things like that works in language like java (ASM)
		// because there is only one compilation target (JAVA Bytecode), it
		// wouldn't be even close to useable in C.
		//
		// Because the arguments never change, when a function is called,
		// the compiler will look at the arguments, and generate the code to
		// set up the stack with enough place for said argument, this means
		// that changing that at runtime is not possible, and as such, the
		// only way to pass an unknown type value as an argument of a
		// variable length function, is to cast it to a type that is bigger.
		// Here intmax_t is used, because the standard guarentees that there
		// isn't any printable (with a printf format) type bigger.
		intmax_t val = *((intmax_t*) vector_get_ptr(vec, i));
		int len = snprintf(NULL, 0, format, val);
		char buf[len + 1];
		snprintf(buf, len + 1, format, val);
		// push formated member
		vector_push_array(&res, buf, len); // not len + 1 we don't want the null termination
		// push delimiter
		vector_push_array(&res, delim, delim_len);
	}
	// remove last delimiter
	vector_pop_many(&res, delim_len, NULL);
	char end[3] = "]\n";
	vector_push_array(&res, end, 3);
	vector_shrink(&res);
	return (char*) res.data;
}
void vector_printf(Vector* vec, char* format) {
	char* str = vector_to_string(vec, format);
	printf("%s", str);
	free(str);
}
Vector vector_from_array(void* data, size_t length, size_t data_size) {
	Vector res = vector_new(length, data_size);
	vector_push_array(&res, data, length);
	return res;
}
