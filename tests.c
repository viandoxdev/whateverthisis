//% #[GLOBAL]
#include <assert.h>
//% #[TEST] [vector]
//% #[general]
#include "vector.h"
#include <string.h>
#include <time.h>

Vector vdef() {
	return vector_new(10, sizeof(int));
}

//% #[tests]

void push_array() {
	Vector v = vdef();
	int arr[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	vector_push_array(&v, arr, 12);
	char* str = vector_to_string(&v, "%d");
	assert(strcmp(str, "[1,2,3,4,5,6,7,8,9,11,12]"));
	free(str);
}

void long_test() {
	struct timespec ts = {};
	ts.tv_nsec = 50000000;
	ts.tv_sec = 1;
	nanosleep(&ts, NULL);
}

void failing_test() {
	assert(1 == 0);
}
void dummy1() {
	assert(1 == 1);
}
void dummy2() {
	assert(1 == 1);
}
void dummy3() {
	assert(1 == 1);
}
void dummy4() {
	assert(1 == 1);
}
void dummy5() {
	assert(1 == 1);
}
void dummy6() {
	assert(1 == 1);
}

//% #[TEST] [genvec]
//% #[general]
#include "genvec.h"
//% #[tests]
void get_out_of_range() {
	GenVec v = genvec_new(5, sizeof(int));
	assert(!genvec_has(&v, 1));
}
