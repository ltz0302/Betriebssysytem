#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "halde.h"



int main(int argc, char *argv[]) {

	
	char* m1 = halde_malloc(512);
        char* m2 = halde_malloc(1024*1024-512-48);
	halde_print();
	halde_free(m1);
	char* m3 = halde_malloc(512);
	halde_free(m2);
	halde_free(m3);

	exit(EXIT_SUCCESS);
}
