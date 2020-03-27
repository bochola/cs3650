#include <stdlib.h>
#include "xmalloc.h"



int
main(int argc, char* argv[]) {
	long* test = xmalloc(47000000);
	long* new_test = xrealloc(test, 259);
	xfree(new_test);
	xfree(test);
	return 0;
}

