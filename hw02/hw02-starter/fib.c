
#include <stdio.h>
#include <stdlib.h>
#include "cfib.c"

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s N, where N >= 0\n", argv[0]);
        return 1;
    }
    
    int value = atol(argv[1]);

    if (value < 0) {
        printf("Usage: ./fib N, where N >= 0\n");
        return 1;
    }
    
    int ans = cfib(value);

    printf("fib(%ld) = %ld\n", atol(argv[1]), ans);
    return 0;
}
