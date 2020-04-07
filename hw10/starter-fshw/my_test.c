#include <stdio.h>
#include "bitmap.h"

int main(int argc, char* argv[]) {
    
    char bm[] = "hello";

    int get = bitmap_get(bm, 9);
    printf("Value is %d\n", get);
    
    bitmap_set(bm, 9, 1);
    printf("After the set\n");
    
    int new_get = bitmap_get(bm, 9);
    printf("Value is %d\n", get);
    printf("New word is %s\n", bm);

    return 0;
}

