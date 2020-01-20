
#include <stdio.h>
#include <stdlib.h>

int cfib(int);

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage:\n  %s N, where N > 0\n", argv[0]);
        return 0;
    }
    
    if (argv[1] < 0) {
        printf("Usage: ./fib N, where N >= 0\n");
        return 0;
    }

    printf("fib(%ld) = %ld\n", atol(argv[1]), atol(argv[1]));
    return 0;
}
