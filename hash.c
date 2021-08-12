// this program just serves as an interface between bash scripts and the default_hash function of hashmap
// it is used to get hashes of strings at compile time in the make_components.sh script.

#include "hashmap.h"
#include <stdio.h>
int main(int argc, const char *argv[]) {
	if(argc < 2) return 1;
	int len = strlen(argv[1]);
	printf("%lu\n", default_hash(argv[1], len * sizeof(char)));
	return 0;
}
