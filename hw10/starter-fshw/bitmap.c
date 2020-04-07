// bitmap.c
#include <stdio.h>
#include <stdint.h>
#include "bitmap.h"


int bitmap_get(void* bm, int block_num) {
    
    uint8_t* arr = bm;
    
    int macro = block_num / 8;
    int index = block_num % 8;

    uint8_t letter = arr[macro];

    uint8_t status = (letter >> index) & 1;
    
    return (int) status;
    
}

void bitmap_set(void* bm, int block_num, int val) {
    
    uint8_t* arr = bm;
    
    int macro = block_num / 8;
    int index = block_num % 8;
    printf("Letter index %d, bit index %d\n", macro, index);
    uint8_t letter = arr[macro];
    printf("Letter is %c\n", letter);    
    uint8_t clear_mask = ~(1 << index);
    
    uint8_t cleared = letter & clear_mask;

    uint8_t set_mask = (!!val) << index;

    uint8_t new = cleared | set_mask;

    printf("New letter is %c\n", new);    
    arr[macro] = new;
        
}

void bitmap_print(void* bmi, int size) {
    
    // Not implemented rn
}

