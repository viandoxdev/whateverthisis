/*	this is the test file of this project, it is parsed by make_test.sh
 *  which split this file and makes one executable per test. (there is no
 *  error handeling in case the syntax is wrong so pay attention).
 *
 *	The syntax is simple: 
 *		the file is split in different sections, those are delimited by
 *		#[section name] [argument] (note: argument is only for TEST sections)
 *		since this syntax is seen as an error in clangd, you can prefix it by
 *		"//% ", this gives the same output without the error.
 *		
 *	the different sections are:
 *		Main sections:
 *
 *			GLOBAL: this section comes at the top of the file, code written in it
 *				will be added to the top of every test, this is used for includes like
 *				assert.h or helper function used by every (or most of) tests.
 *
 *			TEST: this section is the only one taking an argument. The argument
 *				describes the name of the tests, for exemple you'd write:
 *				"//% #[TEST] [mylib]"
 *				for the section with tests for "mylib"
 *
 *		SubSections: (those are all after a TEST section)
 *
 *			general: this section is the same as GLOBAL, except it only applies for
 *				tests in the same TEST section, for example that is where you would
 *				put #include "mylib.h". This code will be put at the top of every
 *				test of the same section, just under GLOBAL.
 *
 *			tests: this section is where you will write the actuall tests for the
 *				current section. Tests take the form of void(void) function with no
 *				return (important). for example a test for additions would be written:
 *					void addition() {
 *						assert(1 + 1 == 2);
 *					}
 *				(underscores can be added to the begining of function names, to avoid 
 *				colisions, theses will be ignored. Names starting by /_+[0-9]/ are invalid)
 *				A test is considered failed if and only if the program crashes (through
 *				an invalid assert for example)
 *
 *	That is basically all about this file, please don't read make_tests.sh it is
 *	an abomination of a bash script that somehows passes the shellcheck. (you can
 *	read the produced files in ./$(BIN)/tests though.
 */
//% #[GLOBAL]
#include <assert.h>
//% #[TEST] [vector]
//% #[general]
#include "vector.h"

Vector vdef(int l) {
	if(l) {
		Vector r = vector_new(l*2, sizeof(int));
		for(int i = 1; i <= l; i++) {
			vector_push(&r, &i);
		}
		return r;
	} else {
		return vector_new(10, sizeof(int));
	}
}

void assv(Vector v, char* format, char* res) {
	char* str = vector_to_string(&v, format);
	assert(strcmp(str, res));
	free(str);
}

//% #[tests]

void control() {
	Vector v = vdef(10);
	assv(v, "%d", "[1,2,3,4,5,6,7,8,9,10]");
}

void get_pointer() {
	Vector v = vdef(5);
	int* ptr = vector_get_ptr(&v, 4);
	assert(*ptr == 5);
}

void from_array() {
	int arr[5] = {1, 2, 3, 4, 5};
	Vector v = vector_from_array(arr, 5, sizeof(int));
	assv(v, "%d", "[1,2,3,4,5]");
}

void push() {
	Vector v = vdef(0);
	int a = 1;
	vector_push(&v, &a);
	assv(v, "%d", "[1]");
	a = 4;
	vector_push(&v, &a);
	assv(v, "%d", "[1,4]");
}

void unshift() {
	Vector v = vdef(2);
	int a = 0;
	vector_unshift(&v, &a);
	assv(v, "%d", "[0,1,2]");
}

void insert() {
	Vector v = vdef(4);
	int a = 0;
	vector_insert(&v, 2, &a);
	assv(v, "%d", "[1,2,0,3,4]");
}

void push_array() {
	Vector v = vdef(3);
	int arr[4] = {4, 5, 6, 7};
	vector_push_array(&v, arr, 12);
	assv(v, "%d", "[1,2,3,4,5,6,7]");
}

void unshift_array() {
	Vector v = vdef(3);
	int arr[3] = {4, 5, 6};
	vector_unshift_array(&v, arr, 3);
	assv(v, "%d", "[4,5,6,1,2,3]");
}

void insert_array() {
	Vector v = vdef(4);
	int arr[2] = {1, 4};
	vector_insert_array(&v, 2, arr, 2);
	assv(v, "%d", "[1,2,1,3,4,3]");
}

void pop() {
	Vector v = vdef(3);
	int r;
	vector_pop(&v, &r);
	assv(v, "%d", "[1,2]");
	assert(r == 3);
	// should not crash when data is NULL
	vector_pop(&v, NULL);
}

void shift() {
	Vector v = vdef(3);
	int r;
	vector_shift(&v, &r);
	assv(v, "%d", "[2,3]");
	assert(r == 1);
	vector_shift(&v, NULL);
}

void remove() {
	Vector v = vdef(3);
	int r;
	vector_remove(&v, 1, &r);
	assv(v, "%d", "[1,3]");
	assert(r == 2);
	vector_remove(&v, 0, NULL);
}

void pop_many() {
	Vector v = vdef(10);
	Vector r = vdef(5);
	vector_pop_many(&v, 5, r.data);
	assv(v, "%d", "[1,2,3,4,5]");
	assv(r, "%d", "[6,7,8,9,10]");
	vector_pop_many(&v, 2, NULL);
}

void shift_many() {
	Vector v = vdef(10);
	Vector r = vdef(5);
	vector_shift_many(&v, 5, r.data);
	assv(v, "%d", "[6,7,8,9,10]");
	assv(r, "%d", "[1,2,3,4,5]");
	vector_shift_many(&v, 2, NULL);
}

void remove_many() {
	Vector v = vdef(9);
	Vector r = vdef(3);
	vector_remove_many(&v, 3, 3, r.data);
	assv(v, "%d", "[1,2,3,7,8,9]");
	assv(r, "%d", "[1,2,3]");
	vector_remove_many(&v, 2, 2, NULL);
	assv(v, "%d", "[1,2,8,9]");
}

void shrink() {
	Vector v = vector_new(500, sizeof(int));
	int arr[5] = {1,2,3,4,5};
	vector_push_array(&v, arr, 5);
	assv(v, "%d", "[1,2,3,4,5]");
	vector_shrink(&v);
	assert(v.allocated == 5);
	assv(v, "%d", "[1,2,3,4,5]");
}

void replace() {
	Vector v = vdef(3);
	assert(2 == *(int*)vector_get_ptr(&v, 1));
	int a = 0;
	vector_replace(&v, &a, 1);
	assert(0 == *(int*)vector_get_ptr(&v, 1));
}

void replace_many() {
	Vector v = vdef(5);
	int arr[3] = {7,7,7};
	vector_replace_many(&v, arr, 1, 3);
	assv(v, "%d", "[1,7,7,7,5]");
}

void copy() {
	Vector v = vdef(2);
	int a;
	vector_copy(&v, 1, &a);
	assert(a == 2);
}

void copy_many() {
	Vector v = vdef(5);
	int arr[5];
	vector_copy_many(&v, 0, 5, arr);
	Vector r = vector_from_array(arr, 5, sizeof(int));
	assv(r, "%d", "[1,2,3,4,5]");
}

void swap() {
	Vector v = vdef(3);
	int a = 5;
	vector_swap(&v, 1, &a);
	int r = *(int*)vector_get_ptr(&v, 1);
	assert(r == 5);
	assert(a == 2);
}

void swap_many() {
	Vector v = vdef(5);
	int arr[5] = {5,4,3,2,1};
	vector_swap_many(&v, 0, 5, arr);
	Vector r = vector_from_array(arr, 5, sizeof(int));
	assv(v, "%d", "[5,4,3,2,1]");
	assv(r, "%d", "[1,2,3,4,5]");
}

void concat() {
	Vector v1 = vdef(3);
	Vector v2 = vdef(3);
	vector_concat(&v1, &v2);
	assv(v1, "%d", "[1,2,3,1,2,3]");
}

void clone() {
	Vector v = vdef(3);
	Vector c = vector_clone(&v);
	assv(v, "%d", "[1,2,3]");
	assv(c, "%d", "[1,2,3]");
}

void has() {
	Vector v = vdef(3);
	assert(vector_has(&v, 2));
	assert(!vector_has(&v, 3));
}

//% #[TEST] [genvec]
//% #[general]
#include "genvec.h" 
//% #[tests]
void get_out_of_range() {
	GenVec v = genvec_new(5, sizeof(int));
	assert(!genvec_has(&v, 1));
}
